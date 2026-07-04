// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.

#pragma once

#include "GameplayTagContainer.h"
#include "Engine/StreamableManager.h"

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayAudioSubsystem.generated.h"

struct FSoundDefinition;
class USoundStack;
class USoundSubmix;
class UAudioComponent;
class USoundControlBus;
class UAudioPoolObject;
class UGameplayAudioSettings;
class USoundEffectSubmixPreset;

UCLASS()
class SHADOWSTEP_API UGameplayAudioSubsystem : public UGameInstanceSubsystem {
	GENERATED_BODY()
	
	friend class UGameplayAudioHelper;
	friend class UAudioPoolObject;

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:
	// Playback
	UAudioComponent* PlaySoundByTag(FGameplayTag SoundTag, FVector Location, USceneComponent* AttachTo = nullptr, FName SocketName = NAME_None);
	void PlaySoundByTag(UAudioComponent* ExistingAudioComponent, FGameplayTag SoundTag, FVector Location, USceneComponent* AttachTo = nullptr, FName SocketName = NAME_None);
	void PlaySound2DByTag(FGameplayTag SoundTag, float Volume = 1.0f, float Pitch = 1.0f, bool bPersistentBetweenLevels = true);
	void PlaySound2DByTag(UAudioComponent* ExistingAudioComponent, FGameplayTag SoundTag, float Volume = 1.0f, float Pitch = 1.0f, bool bPersistentBetweenLevels = true);
	void PlaySound2DBySource(TSoftObjectPtr<USoundBase> SoundBase, float Volume = 1.0f, float Pitch = 1.0f, bool bPersistentBetweenlevels = true);
	void PlayMusicByTag(FGameplayTag MusicTag, float FadeDuration = 1.0f);
	UAudioComponent* PlaySurfaceSoundByTag(TEnumAsByte<EPhysicalSurface> Surface, FGameplayTag SoundTag, FVector Location, USceneComponent* AttachTo = nullptr, FName SocketName =  NAME_None);
	void StopMusic(float FadeDuration = 1.0f);

	void PushSoundStack(USoundStack* NewStack, bool bMoveToTopIfActive = false);
	void PopSoundStack(USoundStack* Stack);
	void ReturnToPool(UAudioPoolObject* PoolObject);

	// Callbacks
	void OnStackLoaded(USoundStack* LoadedStack);
	//void OnLevelLoaded(FLevelLoadingData PreviousLevelLoadingData, FLevelLoadingData NewLevelLoadingData);

	// Helpers
	const FSoundDefinition* FindSoundDefinition(FGameplayTag OfTag);
	void ApplyAllBusVolumes(float FadeTime);

	// Effects
	void SetSubmixEffects(USoundSubmix* ToSubmix, USoundEffectSubmixPreset* Effect, float FadeTimeSeconds);
	void ClearSubmixEffects(USoundSubmix* FromSubmix, float FadeTimeSeconds);

	// Getters/Setters
	void SetVolume(const UObject* WorldContextObject, TSoftObjectPtr<USoundControlBus> OfBus, float Volume);
	float GetVolume(TSoftObjectPtr<USoundControlBus> OfBus) const;
	USoundBase* GetSoundFromTag(FGameplayTag SoundTag);

private:
	UPROPERTY()
	TArray<TObjectPtr<USoundStack>> ActiveStacks{};

	// No need to use 'UPROPERTY' here -> The sound stacks are kept alive by 'ActiveStacks'
	TMap<TObjectPtr<USoundStack>, TSharedPtr<FStreamableHandle>> StackBatchHandles{};

	UPROPERTY()
	TMap<TSoftObjectPtr<USoundControlBus>, float> BusVolumeMap{};

	UPROPERTY()
	TObjectPtr<class USoundControlBusMix> DesignerControlBusMix{ nullptr };

	// Memory Management
	FStreamableManager StreamableManager{};

	// Pool
	UPROPERTY()
	TArray<TObjectPtr<UAudioPoolObject>> ActivePool{};

	UPROPERTY()
	TArray<TObjectPtr<UAudioPoolObject>> FreePool{};

	// Music
	TWeakObjectPtr<UAudioComponent> CurrentMusicComponent{ nullptr };
	FGameplayTag CurrentMusicTag{};

	UPROPERTY();
	const UGameplayAudioSettings* GameplayAudioSettings{ nullptr };
};
