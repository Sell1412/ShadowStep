// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.

#include "Audio/GameplayAudioSubsystem.h"

#include "GameplayAudioSettings.h"
#include "Sound/SoundSubmix.h"
#include "ShadowStep.h"
#include "SoundStack.h"
#include "AudioModulationStatics.h"
#include "SoundControlBus.h"
#include "LevelStackMap.h"
#include "LevelMusicMap.h"
#include "AudioPoolObject.h"
#include "Kismet/GameplayStatics.h"
#include "AudioMixerBlueprintLibrary.h"

void UGameplayAudioSubsystem::Initialize(FSubsystemCollectionBase& Collection) {
	Super::Initialize(Collection);

	// Cache the gameplay audio settings
	GameplayAudioSettings = GetDefault<UGameplayAudioSettings>();

	if (GameplayAudioSettings ) {
		for (auto& BusVolumePair : GameplayAudioSettings->DefaultBusVolumes) {
			BusVolumeMap.Add(BusVolumePair.Key, BusVolumePair.Value); // Cache -> don't SetVolume() here, since world might not be valid yet
		}
	}

	// Make sure the loading system is initialized, so we can subscribe to its event
	//UGameLoadingSubsystem* LoadingSubsystem = Collection.InitializeDependency<UGameLoadingSubsystem>();
	//if (LoadingSubsystem) {
	//	LoadingSubsystem->OnLoadingFinished.SubscribeUObject(this, &UGameplayAudioSubsystem::OnLevelLoaded);
	//}
}

void UGameplayAudioSubsystem::Deinitialize() {
	StackBatchHandles.Empty(); // Release memory
	Super::Deinitialize();
}

UAudioComponent* UGameplayAudioSubsystem::PlaySoundByTag(FGameplayTag SoundTag, FVector Location, USceneComponent* AttachTo /*= nullptr*/, FName SocketName /*= NAME_None*/) {
	if (!GameplayAudioSettings) return nullptr;
	
	const FSoundDefinition* Definition = FindSoundDefinition(SoundTag);
	if (!Definition) {
		ULOG("Sound {0} couldn't be found in any active stack!", SoundTag.GetTagName()); return nullptr;
	}

	if (Definition && Definition->SoundFile.IsNull()) {
		ULOG("Sound {0} doesn't have an assigned sound asset!", SoundTag.GetTagName()); return nullptr;
	}

	USoundBase* SoundToPlay = Definition->SoundFile.LoadSynchronous(); // Should already be loaded by stack.
	if (!SoundToPlay) return nullptr;

	// Handle pooling
	UAudioPoolObject* PoolObject{ nullptr };
	if (FreePool.Num() > 0)
		PoolObject = FreePool.Pop();
	// Create a new pool object is we haven't reached the maximum
	else if (FreePool.Num() + ActivePool.Num() < GameplayAudioSettings->MaximumPoolItems) {
		PoolObject = NewObject<UAudioPoolObject>(this);
		PoolObject->Init(this);
	}
	else { // Reached maximum capacity
		return nullptr;
	}

	ActivePool.Add(PoolObject);

	// Play
	if (AttachTo) return PoolObject->PlayAttached(SoundToPlay, AttachTo, SocketName, Definition->AttenuationOverride, Definition->ConcurrencyOverride);
	else PoolObject->Play(SoundToPlay, Location, Definition->AttenuationOverride, Definition->ConcurrencyOverride);

	return nullptr;
}

void UGameplayAudioSubsystem::PlaySoundByTag(UAudioComponent* ExistingAudioComponent, FGameplayTag SoundTag, FVector Location, USceneComponent* AttachTo /*= nullptr*/, FName SocketName /*= NAME_None*/) {
	if (!ExistingAudioComponent) return;

	// TODO: If the existing audio component is from the pool, this might lead to problems -> in this case we might want to handle it differently and use the custom pool object Play()
	
	const FSoundDefinition* Definition = FindSoundDefinition(SoundTag);
	if (!Definition) {
		ULOG("Sound {0} couldn't be found in any active stack!", SoundTag.GetTagName()); return;
	}

	if (Definition && Definition->SoundFile.IsNull()) {
		ULOG("Sound {0} doesn't have an assigned sound asset!", SoundTag.GetTagName()); return;
	}

	USoundBase* SoundToPlay = Definition->SoundFile.LoadSynchronous(); // Should already be loaded by stack.
	if (!SoundToPlay) return;

	ExistingAudioComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	ExistingAudioComponent->SetSound(SoundToPlay);
	ExistingAudioComponent->AttenuationSettings = Definition->AttenuationOverride;
	ExistingAudioComponent->bIgnoreForFlushing = false;

	ExistingAudioComponent->ConcurrencySet.Empty();
	if (Definition->ConcurrencyOverride) {
		ExistingAudioComponent->ConcurrencySet.Add(Definition->ConcurrencyOverride);
	}

	// Make sure to reset multipliers
	ExistingAudioComponent->SetVolumeMultiplier(1.0f);
	ExistingAudioComponent->SetPitchMultiplier(1.0f);

	if (AttachTo) { // Handle attach logic
		ExistingAudioComponent->AttachToComponent(AttachTo, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
	}
	else { // Handle location logic
		ExistingAudioComponent->SetWorldLocation(Location);
	}

	ExistingAudioComponent->Play();
}

void UGameplayAudioSubsystem::PlaySound2DByTag(FGameplayTag SoundTag, float Volume /*= 1.0f*/, float Pitch /*= 1.0f*/, bool bPersistentBetweenLevels /*= true*/) {
	const FSoundDefinition* Definition = FindSoundDefinition(SoundTag);
	if (!Definition) {
		ULOG("Sound {0} couldn't be found in any active stack!", SoundTag.GetTagName()); return;
	}
	if (Definition && Definition->SoundFile.IsNull()) {
		ULOG("Sound {0} doesn't have an assigned sound asset!", SoundTag.GetTagName()); return;
	}

	USoundBase* SoundToPlay = Definition->SoundFile.LoadSynchronous(); // Should already be loaded by stack.
	if (!SoundToPlay) return;

	UAudioComponent* AudioComponent = UGameplayStatics::CreateSound2D(
		GetWorld(),
		SoundToPlay,
		Volume,
		Pitch,
		0.0f,
		Definition->ConcurrencyOverride,
		bPersistentBetweenLevels,
		true
	);

	if (AudioComponent) AudioComponent->Play();
}

void UGameplayAudioSubsystem::PlaySound2DByTag(UAudioComponent* ExistingAudioComponent, FGameplayTag SoundTag, float Volume /*= 1.0f*/, float Pitch /*= 1.0f*/, bool bPersistentBetweenLevels /*= true*/) {
	if (!ExistingAudioComponent) return;
	
	// TODO: If the existing audio component is from the pool, this might lead to problems -> in this case we might want to handle it differently and use the custom pool object Play()

	const FSoundDefinition* Definition = FindSoundDefinition(SoundTag);
	if (!Definition) {
		ULOG("Sound {0} couldn't be found in any active stack!", SoundTag.GetTagName()); return;
	}
	if (Definition && Definition->SoundFile.IsNull()) {
		ULOG("Sound {0} doesn't have an assigned sound asset!", SoundTag.GetTagName()); return;
	}

	USoundBase* SoundToPlay = Definition->SoundFile.LoadSynchronous(); // Should already be loaded by stack.
	if (!SoundToPlay) return;

	// Update values
	ExistingAudioComponent->SetSound(SoundToPlay);
	ExistingAudioComponent->SetVolumeMultiplier(Volume);
	ExistingAudioComponent->SetPitchMultiplier(Pitch);
	ExistingAudioComponent->ConcurrencySet.Empty();
	ExistingAudioComponent->ConcurrencySet.Add(Definition->ConcurrencyOverride);
	ExistingAudioComponent->bIgnoreForFlushing = bPersistentBetweenLevels;
	ExistingAudioComponent->AttenuationSettings = nullptr; // Clear attenuation, so sound is played 2D
	ExistingAudioComponent->Play();
}

void UGameplayAudioSubsystem::PlaySound2DBySource(TSoftObjectPtr<USoundBase> SoundBase, float Volume /*= 1.0f*/, float Pitch /*= 1.0f*/, bool bPersistentBetweenLevels /*= true*/) {
	if (SoundBase.IsNull()) return;
	USoundBase* SoundToPlay = SoundBase.LoadSynchronous(); // Might already be loaded by a stack
	if (!SoundToPlay) return;

	UAudioComponent* AudioComponent = UGameplayStatics::CreateSound2D(
		GetWorld(),
		SoundToPlay,
		Volume,
		Pitch,
		0.0f,
		nullptr,
		bPersistentBetweenLevels,
		true
	);

	if (AudioComponent) AudioComponent->Play();
}

void UGameplayAudioSubsystem::PlayMusicByTag(FGameplayTag MusicTag, float FadeDuration /*= 1.0f*/) {
	if (CurrentMusicTag == MusicTag && CurrentMusicComponent.IsValid() && CurrentMusicComponent->IsPlaying()) return;

	StopMusic(FadeDuration);

	const FSoundDefinition* Definition = FindSoundDefinition(MusicTag);
	if (!Definition) {
		ULOG("Sound {0} couldn't be found in any active stack!", MusicTag.GetTagName()); return;
	}
	if (Definition && Definition->SoundFile.IsNull()) {
		ULOG("Sound {0} doesn't have an assigned sound asset!", MusicTag.GetTagName()); return;
	}

	USoundBase* MusicToPlay = Definition->SoundFile.LoadSynchronous(); // Should already be loaded by stack.
	if (!MusicToPlay) return;

	if (CurrentMusicComponent.IsValid()) {
		CurrentMusicComponent->SetSound(MusicToPlay);
	}
	// Spawn 2D Sound (if none exists)
	else {
		UAudioComponent* MusicAudioComponent = UGameplayStatics::CreateSound2D(GetWorld(), MusicToPlay);
		CurrentMusicComponent = MusicAudioComponent;
	}

	CurrentMusicComponent->Play();
	CurrentMusicComponent->FadeIn(FadeDuration, 1.0f); // Fade in new music
	CurrentMusicTag = MusicTag;
}

UAudioComponent* UGameplayAudioSubsystem::PlaySurfaceSoundByTag(TEnumAsByte<EPhysicalSurface> Surface, FGameplayTag SoundTag, FVector Location, USceneComponent* AttachTo /*= nullptr*/, FName SocketName /*= NAME_None*/) {
	if (!GameplayAudioSettings) return nullptr;

	const FSoundDefinition* Definition = FindSoundDefinition(SoundTag);
	if (!Definition) {
		ULOG("Sound {0} couldn't be found in any active stack!", SoundTag.GetTagName()); return nullptr;
	}

	if (Definition && (!Definition->SurfaceSoundMap.Contains(Surface) || Definition->SurfaceSoundMap[Surface].IsNull())) {
		ULOG("Sound {0} doesn't have an entry or assigned sound asset for the specified surface type {1}!", SoundTag.GetTagName(), UEnum::GetValueAsString(Surface)); return nullptr;
	}

	USoundBase* SoundToPlay = Definition->SurfaceSoundMap[Surface].LoadSynchronous(); // Should already be loaded by stack.
	if (!SoundToPlay) return nullptr;

	// Handle pooling
	UAudioPoolObject* PoolObject{ nullptr };
	if (FreePool.Num() > 0)
		PoolObject = FreePool.Pop();
	// Create a new pool object is we haven't reached the maximum
	else if (FreePool.Num() + ActivePool.Num() < GameplayAudioSettings->MaximumPoolItems) {
		PoolObject = NewObject<UAudioPoolObject>(this);
		PoolObject->Init(this);
	}
	else { // Reached maximum capacity
		return nullptr;
	}

	ActivePool.Add(PoolObject);

	// Play
	if (AttachTo) return PoolObject->PlayAttached(SoundToPlay, AttachTo, SocketName, Definition->AttenuationOverride, Definition->ConcurrencyOverride);
	else PoolObject->Play(SoundToPlay, Location, Definition->AttenuationOverride, Definition->ConcurrencyOverride);

	return nullptr;
}

void UGameplayAudioSubsystem::StopMusic(float FadeDuration /*= 1.0f*/) {
	if (CurrentMusicComponent.IsValid()) {
		CurrentMusicComponent->FadeOut(FadeDuration, 0.0f);
	}

	CurrentMusicTag = FGameplayTag::EmptyTag;
}

void UGameplayAudioSubsystem::PushSoundStack(USoundStack* NewStack, bool bMoveToTopIfActive /*= false*/) {
	if (!NewStack) return;

	bool bAlreadyActive = ActiveStacks.Contains(NewStack);

	if (bAlreadyActive && bMoveToTopIfActive) {
		ActiveStacks.Remove(NewStack); // Remove so doesn't exist twice
		ActiveStacks.Add(NewStack);
	}
	else if(!bAlreadyActive) {
		ActiveStacks.Add(NewStack);

		// Load Stack
		const auto& AssetsToLoad = NewStack->GetAssetsToLoad();
		if (AssetsToLoad.Num() <= 0) return;

		TSharedPtr<FStreamableHandle> BatchLoadHandle = StreamableManager.RequestAsyncLoad(AssetsToLoad, FStreamableDelegate::CreateUObject(this, &UGameplayAudioSubsystem::OnStackLoaded, NewStack));
		StackBatchHandles.Add(NewStack, BatchLoadHandle); // Store the handle so sounds stay in RAM
	}
	else { // Active and shouldn't be placed on top again
		ULOG("Tried to push an already active stack!");
	}
}

void UGameplayAudioSubsystem::PopSoundStack(USoundStack* Stack) {
	if (!Stack) return;
	ActiveStacks.Remove(Stack);

	// Free sound memory
	if (StackBatchHandles.Contains(Stack))
		StackBatchHandles.Remove(Stack);
}


void UGameplayAudioSubsystem::ReturnToPool(UAudioPoolObject* PoolObject) {
	if (ActivePool.Contains(PoolObject)) {
		ActivePool.Remove(PoolObject);
		FreePool.Add(PoolObject);
	}
}

void UGameplayAudioSubsystem::OnStackLoaded(USoundStack* LoadedStack) {
	ULOG("Finished loading audio stack: {0}!", *LoadedStack->GetName());
}

//void UGameplayAudioSubsystem::OnLevelLoaded(FLevelLoadingData PreviousLevelLoadingData, FLevelLoadingData NewLevelLoadingData) {
//	TSoftObjectPtr<UWorld> LoadedLevel = NewLevelLoadingData.LevelAsset;
//	if (LoadedLevel.IsNull() || !GameplayAudioSettings) return;
//	
//	ApplyAllBusVolumes(0.1f);
//
//	// Check Level-Stack Map
//	ULevelStackMap* LevelStackMap = GameplayAudioSettings->LevelStackMap.LoadSynchronous();
//	if (LevelStackMap) {
//		for (auto& WorldToStacksPair : LevelStackMap->LevelStackMap) {
//			if (WorldToStacksPair.Key == LoadedLevel) { // Level exists in the map
//				for (auto& Stack : WorldToStacksPair.Value.Stacks) { // Load each stack assigned to the level
//					USoundStack* NewStack = Stack.LoadSynchronous();
//					PushSoundStack(NewStack);
//				}
//
//				break; // Found the desired level
//			}
//		}
//	}
//
//	// Check Level-Music Map
//	ULevelMusicMap* LevelMusicMap = GameplayAudioSettings->LevelMusicMap.LoadSynchronous();
//	if (LevelMusicMap) {
//		for (auto& LevelToMusicPair : LevelMusicMap->LevelMusicMap) {
//			if (LevelToMusicPair.Key == LoadedLevel) { // Level exists in the map
//				// Play the music assigned to the level
//				PlayMusicByTag(LevelToMusicPair.Value.MusicTag, LevelToMusicPair.Value.MusicFadeDuration);
//			}
//
//			break; // Found the desired level
//		}
//	}
//}

const FSoundDefinition* UGameplayAudioSubsystem::FindSoundDefinition(FGameplayTag OfTag) {
	for (int32 StackIndex = ActiveStacks.Num() - 1; StackIndex >= 0; --StackIndex) {
		auto& Stack = ActiveStacks[StackIndex];
		if (Stack && Stack->SoundDatabase.Contains(OfTag)) {
			return &Stack->SoundDatabase[OfTag];
		}
	}

	return nullptr; // No sound of the given tag was found in any stack
}

void UGameplayAudioSubsystem::ApplyAllBusVolumes(float FadeTime) {
	UWorld* World = GetWorld();
	if (!World || !GameplayAudioSettings) {
		ULOG("ApplyAllBusVolumes failed: invalid world or settings"); return;
	}

	if (!DesignerControlBusMix && !GameplayAudioSettings->DesignerControlBusMix.IsNull()) {
		DesignerControlBusMix = GameplayAudioSettings->DesignerControlBusMix.LoadSynchronous();
	}

	if (DesignerControlBusMix) {
		UAudioModulationStatics::ActivateBusMix(World, DesignerControlBusMix);
	}

	for (const auto& Pair : BusVolumeMap) {
		USoundControlBus* LoadedBus = Pair.Key.LoadSynchronous();
		if (!LoadedBus) continue;
	
		UAudioModulationStatics::SetGlobalBusMixValue(World, LoadedBus, Pair.Value, FadeTime);
	}
}

void UGameplayAudioSubsystem::SetSubmixEffects(USoundSubmix* ToSubmix, USoundEffectSubmixPreset* Effect, float FadeTimeSeconds) {
	if (!ToSubmix || !Effect) return;

	if (UWorld* World = GetWorld()) {
		TArray<USoundEffectSubmixPreset*> NewChain{};
		NewChain.Add(Effect);
		UAudioMixerBlueprintLibrary::SetSubmixEffectChainOverride(World, ToSubmix, NewChain, FadeTimeSeconds);
	}
}

void UGameplayAudioSubsystem::ClearSubmixEffects(USoundSubmix* FromSubmix, float FadeTimeSeconds) {
	if (!FromSubmix) return;

	if (UWorld* World = GetWorld()) {
		UAudioMixerBlueprintLibrary::ClearSubmixEffectChainOverride(World, FromSubmix, FadeTimeSeconds);
	}
}

void UGameplayAudioSubsystem::SetVolume(const UObject* WorldContextObject, TSoftObjectPtr<USoundControlBus> OfBus, float Volume) {
	if (OfBus.IsNull()) {
		ULOG("SetVolume failed: bus is null"); return;
	}

	BusVolumeMap.Add(OfBus, Volume);

	UWorld* World = WorldContextObject ? WorldContextObject->GetWorld() : GetWorld();
	if (!World) return;

	if (USoundControlBus* LoadedBus = OfBus.LoadSynchronous()) {
		UAudioModulationStatics::SetGlobalBusMixValue(World, LoadedBus, Volume, 0.1f);
	}
}

float UGameplayAudioSubsystem::GetVolume(TSoftObjectPtr<USoundControlBus> OfBus) const {
	if (OfBus.IsNull()) return 0.0f;
	if (const float* FoundVolume = BusVolumeMap.Find(OfBus)) {
		return *FoundVolume;
	}
	return 0.0f;
}

USoundBase* UGameplayAudioSubsystem::GetSoundFromTag(FGameplayTag SoundTag) {
	const FSoundDefinition* Definition = FindSoundDefinition(SoundTag);
	if (!Definition) {
		ULOG("Sound {0} couldn't be found in any active stack!", SoundTag.GetTagName()); return nullptr;
	}

	if (Definition && Definition->SoundFile.IsNull()) {
		ULOG("Sound {0} doesn't have an assigned sound asset!", SoundTag.GetTagName()); return nullptr;
	}

	USoundBase* SoundToPlay = Definition->SoundFile.LoadSynchronous(); // Should already be loaded by stack.
	if (!SoundToPlay) return nullptr;

	return SoundToPlay;
}
