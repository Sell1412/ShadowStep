// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.

#include "Audio/GameplayAudioHelper.h"

#include "GameplayAudioSubsystem.h"
#include "Components/AudioComponent.h"

UGameplayAudioSubsystem* UGameplayAudioHelper::GetAudioSubsystem(const UObject* WorldContextObject) {
	if (!WorldContextObject) return nullptr;

	UWorld* World = WorldContextObject->GetWorld();
	if (!World) return nullptr;

	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance) return nullptr;

	return GameInstance->GetSubsystem<UGameplayAudioSubsystem>();
}

UGameplayAudioSubsystem* UGameplayAudioHelper::GetAudioSubsystem(const UWorld* World) {
	if (!World) return nullptr;

	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance) return nullptr;

	return GameInstance->GetSubsystem<UGameplayAudioSubsystem>();
}

void UGameplayAudioHelper::PushSoundStack(const UObject* WorldContextObject, USoundStack* Stack, bool bMoveToTopIfActive /*= false*/) {
	if (UGameplayAudioSubsystem* GameplayAudioSubsystem = GetAudioSubsystem(WorldContextObject)) {
		GameplayAudioSubsystem->PushSoundStack(Stack, bMoveToTopIfActive);
	}
}

void UGameplayAudioHelper::PopSoundStack(const UObject* WorldContextObject, USoundStack* Stack) {
	if (UGameplayAudioSubsystem* GameplayAudioSubsystem = GetAudioSubsystem(WorldContextObject)) {
		GameplayAudioSubsystem->PopSoundStack(Stack);
	}
}

void UGameplayAudioHelper::PlaySoundAtLocation(const UObject* WorldContextObject, FGameplayTag SoundTag, FVector Location) {
	if (UGameplayAudioSubsystem* GameplayAudioSubsystem = GetAudioSubsystem(WorldContextObject)) {
		GameplayAudioSubsystem->PlaySoundByTag(SoundTag, Location);
	}
}

void UGameplayAudioHelper::PlaySoundAtLocationWithExistingComponent(UAudioComponent* ExistingAudioComponent, FGameplayTag SoundTag, FVector Location) {
	if (UGameplayAudioSubsystem* GameplayAudioSubsystem = GetAudioSubsystem(ExistingAudioComponent->GetWorld())) {
		GameplayAudioSubsystem->PlaySoundByTag(ExistingAudioComponent, SoundTag, Location);
	}
}

void UGameplayAudioHelper::PlaySurfaceSoundAtLocation(const UObject* WorldContextObject, TEnumAsByte<EPhysicalSurface> Surface, FGameplayTag SoundTag, FVector Location) {
	if (UGameplayAudioSubsystem* GameplayAudioSubsystem = GetAudioSubsystem(WorldContextObject)) {
		GameplayAudioSubsystem->PlaySurfaceSoundByTag(Surface, SoundTag, Location);
	}
}

UAudioComponent* UGameplayAudioHelper::PlaySurfaceSoundAttached(const UObject* WorldContextObject, TEnumAsByte<EPhysicalSurface> Surface, FGameplayTag SoundTag, USceneComponent* AttachTo, FName SocketName /*= NAME_None*/) {
	if (UGameplayAudioSubsystem* GameplayAudioSubsystem = GetAudioSubsystem(WorldContextObject)) {
		return GameplayAudioSubsystem->PlaySurfaceSoundByTag(Surface, SoundTag, FVector::ZeroVector, AttachTo, SocketName);
	}
	return nullptr;
}

UAudioComponent* UGameplayAudioHelper::PlaySoundAttached(const UObject* WorldContextObject, FGameplayTag SoundTag, USceneComponent* AttachTo, FName SocketName /*= NAME_None*/) {
	if (UGameplayAudioSubsystem* GameplayAudioSubsystem = GetAudioSubsystem(WorldContextObject)) {
		return GameplayAudioSubsystem->PlaySoundByTag(SoundTag, FVector::ZeroVector, AttachTo, SocketName);
	}
	return nullptr;
}

void UGameplayAudioHelper::PlaySoundAttachedWithExistingComponent(UAudioComponent* ExistingAudioComponent, FGameplayTag SoundTag, USceneComponent* AttachTo, FName SocketName /*= NAME_None*/) {
	if (UGameplayAudioSubsystem* GameplayAudioSubsystem = GetAudioSubsystem(ExistingAudioComponent->GetWorld())) {
		GameplayAudioSubsystem->PlaySoundByTag(ExistingAudioComponent, SoundTag, FVector::ZeroVector, AttachTo, SocketName);
	}
}

UAudioComponent* UGameplayAudioHelper::PlaySoundAttachedToActor(FGameplayTag SoundTag, AActor* AttachTo, FName SocketName /*= NAME_None*/) {
	if (!AttachTo) return nullptr;
	
	if (UGameplayAudioSubsystem* GameplayAudioSubsystem = GetAudioSubsystem(AttachTo->GetWorld())) {
		return GameplayAudioSubsystem->PlaySoundByTag(SoundTag, FVector::ZeroVector, AttachTo->GetRootComponent(), SocketName);
	}
	return nullptr;
}

void UGameplayAudioHelper::PlaySound2D(const UObject* WorldContextObject, FGameplayTag SoundTag, float Volume /*= 1.0f*/, float Pitch /*= 1.0f*/, bool bPersistentBetweenLevels /*= true*/) {
	if (UGameplayAudioSubsystem* GameplayAudioSubsystem = GetAudioSubsystem(WorldContextObject)) {
		GameplayAudioSubsystem->PlaySound2DByTag(SoundTag, Volume, Pitch, bPersistentBetweenLevels);
	}
}

void UGameplayAudioHelper::PlaySound2DWithExistingComponent(UAudioComponent* ExistingAudioComponent, FGameplayTag SoundTag, float Volume /*= 1.0f*/, float Pitch /*= 1.0f*/, bool bPersistentBetweenLevels /*= true*/) {
	if (UGameplayAudioSubsystem* GameplayAudioSubsystem = GetAudioSubsystem(ExistingAudioComponent->GetWorld())) {
		GameplayAudioSubsystem->PlaySound2DByTag(ExistingAudioComponent, SoundTag, Volume, Pitch, bPersistentBetweenLevels);
	}
}

void UGameplayAudioHelper::PlaySound2DBySource(const UObject* WorldContextObject, TSoftObjectPtr<USoundBase> SoundBase, float Volume /*= 1.0f*/, float Pitch /*= 1.0f*/, bool bPersistentBetweenLevels /*= true*/) {
	if (UGameplayAudioSubsystem* GameplayAudioSubsystem = GetAudioSubsystem(WorldContextObject)) {
		GameplayAudioSubsystem->PlaySound2DBySource(SoundBase, Volume, Pitch, bPersistentBetweenLevels);
	}
}

void UGameplayAudioHelper::PlayMusic(const UObject* WorldContextObject, FGameplayTag MusicTag, float FadeDuration /*= 2.0f*/) {
	if (UGameplayAudioSubsystem* GameplayAudioSubsystem = GetAudioSubsystem(WorldContextObject)) {
		GameplayAudioSubsystem->PlayMusicByTag(MusicTag, FadeDuration);
	}
}

void UGameplayAudioHelper::StopMusic(const UObject* WorldContextObject, float FadeDuration /*= 1.0f*/) {
	if (UGameplayAudioSubsystem* GameplayAudioSubsystem = GetAudioSubsystem(WorldContextObject)) {
		GameplayAudioSubsystem->StopMusic(FadeDuration);
	}
}

void UGameplayAudioHelper::SetVolume(const UObject* WorldContextObject, TSoftObjectPtr<USoundControlBus> OfBus, float Volume) {
	if (UGameplayAudioSubsystem* GameplayAudioSubsystem = GetAudioSubsystem(WorldContextObject)) {
		GameplayAudioSubsystem->SetVolume(WorldContextObject, OfBus, Volume);
	}
}

float UGameplayAudioHelper::GetVolume(const UObject* WorldContextObject, TSoftObjectPtr<USoundControlBus> OfBus) {
	if (UGameplayAudioSubsystem* GameplayAudioSubsystem = GetAudioSubsystem(WorldContextObject)) {
		return GameplayAudioSubsystem->GetVolume(OfBus);
	}

	return 0.0f;
}

USoundBase* UGameplayAudioHelper::GetSoundFromTag(const UObject* WorldContextObject, FGameplayTag SoundTag) {
	if (UGameplayAudioSubsystem* GameplayAudioSubsystem = GetAudioSubsystem(WorldContextObject)) {
		return GameplayAudioSubsystem->GetSoundFromTag(SoundTag);
	}

	return nullptr;
}

void UGameplayAudioHelper::SetSubmixEffects(const UObject* WorldContextObject, USoundSubmix* ToSubmix, USoundEffectSubmixPreset* Effect, float FadeTimeSeconds) {
	if (UGameplayAudioSubsystem* GameplayAudioSubsystem = GetAudioSubsystem(WorldContextObject)) {
		return GameplayAudioSubsystem->SetSubmixEffects(ToSubmix, Effect, FadeTimeSeconds);
	}
}

void UGameplayAudioHelper::ClearSubmixEffects(const UObject* WorldContextObject, USoundSubmix* FromSubmix, float FadeTimeSeconds) {
	if (UGameplayAudioSubsystem* GameplayAudioSubsystem = GetAudioSubsystem(WorldContextObject)) {
		return GameplayAudioSubsystem->ClearSubmixEffects(FromSubmix, FadeTimeSeconds);
	}
}
