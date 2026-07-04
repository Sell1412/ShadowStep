// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.

#pragma once

#include "GameplayTagContainer.h"

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayAudioHelper.generated.h"

class UGameplayAudioSubsystem;

class USoundBase;
class USoundStack;
class UAudioComponent;
class USoundControlBus;

UCLASS()
class SHADOWSTEP_API UGameplayAudioHelper : public UBlueprintFunctionLibrary {
	GENERATED_BODY()

public:
	static UGameplayAudioSubsystem* GetAudioSubsystem(const UObject* WorldContextObject);
	static UGameplayAudioSubsystem* GetAudioSubsystem(const UWorld* World);

	UFUNCTION(BlueprintCallable, Category = "YATools|Audio", meta = (WorldContext = "WorldContextObject"))
	static void PushSoundStack(const UObject* WorldContextObject, USoundStack* Stack, bool bMoveToTopIfActive = false);

	UFUNCTION(BlueprintCallable, Category = "YATools|Audio", meta = (WorldContext = "WorldContextObject"))
	static void PopSoundStack(const UObject* WorldContextObject, USoundStack* Stack);

	UFUNCTION(BlueprintCallable, Category = "YATools|Audio", meta = (WorldContext = "WorldContextObject"))
	static void PlaySoundAtLocation(const UObject* WorldContextObject, FGameplayTag SoundTag, FVector Location);

	UFUNCTION(BlueprintCallable, Category = "YATools|Audio")
	static void PlaySoundAtLocationWithExistingComponent(UAudioComponent* ExistingAudioComponent, FGameplayTag SoundTag, FVector Location);

	UFUNCTION(BlueprintCallable, Category = "YATools|Audio", meta = (WorldContext = "WorldContextObject"))
	static void PlaySurfaceSoundAtLocation(const UObject* WorldContextObject, TEnumAsByte<EPhysicalSurface> Surface, FGameplayTag SoundTag, FVector Location);

	UFUNCTION(BlueprintCallable, Category = "YATools|Audio", meta = (WorldContext = "WorldContextObject"))
	static UAudioComponent* PlaySurfaceSoundAttached(const UObject* WorldContextObject, TEnumAsByte<EPhysicalSurface> Surface, FGameplayTag SoundTag, USceneComponent* AttachTo, FName SocketName = NAME_None);
	
	UFUNCTION(BlueprintCallable, Category = "YATools|Audio", meta = (WorldContext = "WorldContextObject"))
	static UAudioComponent* PlaySoundAttached(const UObject* WorldContextObject, FGameplayTag SoundTag, USceneComponent* AttachTo, FName SocketName = NAME_None);

	UFUNCTION(BlueprintCallable, Category = "YATools|Audio")
	static void PlaySoundAttachedWithExistingComponent(UAudioComponent* ExistingAudioComponent, FGameplayTag SoundTag, USceneComponent* AttachTo, FName SocketName = NAME_None);

	UFUNCTION(BlueprintCallable, Category = "YATools|Audio", meta = (WorldContext = "WorldContextObject"))
	static UAudioComponent* PlaySoundAttachedToActor(FGameplayTag SoundTag, AActor* AttachTo, FName SocketName = NAME_None);

	UFUNCTION(BlueprintCallable, Category = "YATools|Audio", meta = (WorldContext = "WorldContextObject"))
	static void PlaySound2D(const UObject* WorldContextObject, FGameplayTag SoundTag, float Volume = 1.0f, float Pitch = 1.0f, bool bPersistentBetweenLevels = true);

	UFUNCTION(BlueprintCallable, Category = "YATools|Audio")
	static void PlaySound2DWithExistingComponent(UAudioComponent* ExistingAudioComponent, FGameplayTag SoundTag, float Volume = 1.0f, float Pitch = 1.0f, bool bPersistentBetweenLevels = true);

	UFUNCTION(BlueprintCallable, Category = "YATools|Audio", meta = (DisplayName = "Play Sound 2D By Source", WorldContext = "WorldContextObject"))
	static void PlaySound2DBySource(const UObject* WorldContextObject, TSoftObjectPtr<USoundBase> SoundBase, float Volume = 1.0f, float Pitch = 1.0f, bool bPersistentBetweenLevels = true);

	UFUNCTION(BlueprintCallable, Category = "YATools|Audio", meta = (WorldContext = "WorldContextObject"))
	static void PlayMusic(const UObject* WorldContextObject, FGameplayTag MusicTag, float FadeDuration = 2.0f);

	UFUNCTION(BlueprintCallable, Category = "YATools|Audio", meta = (WorldContext = "WorldContextObject"))
	static void StopMusic(const UObject* WorldContextObject, float FadeDuration = 1.0f);

	UFUNCTION(BlueprintCallable, Category = "YATools|Audio", meta = (WorldContext = "WorldContextObject"))
	static void SetVolume(const UObject* WorldContextObject, TSoftObjectPtr<USoundControlBus> OfBus, float Volume);

	UFUNCTION(BlueprintCallable, Category = "YATools|Audio", meta = (WorldContext = "WorldContextObject"))
	static float GetVolume(const UObject* WorldContextObject, TSoftObjectPtr<USoundControlBus> OfBus);

	UFUNCTION(BlueprintCallable, Category = "YATools|Audio", meta = (WorldContext = "WorldContextObject"))
	static USoundBase* GetSoundFromTag(const UObject* WorldContextObject, FGameplayTag SoundTag);

	UFUNCTION(BlueprintCallable, Category = "YATools|Audio", meta = (WorldContext = "WorldContextObject"))
	static void SetSubmixEffects(const UObject* WorldContextObject, USoundSubmix* ToSubmix, USoundEffectSubmixPreset* Effect, float FadeTimeSeconds);

	UFUNCTION(BlueprintCallable, Category = "YATools|Audio", meta = (WorldContext = "WorldContextObject"))
	static void ClearSubmixEffects(const UObject* WorldContextObject, USoundSubmix* FromSubmix, float FadeTimeSeconds);
};
