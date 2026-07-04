// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AudioPoolObject.generated.h"

class UGameplayAudioSubsystem;
class USoundAttenuation;
class USoundConcurrency;
class UAudioComponent;
class USoundBase;

UCLASS()
class SHADOWSTEP_API UAudioPoolObject : public UObject {
	GENERATED_BODY()
	
	friend class UGameplayAudioSubsystem;

private:
	void Init(UGameplayAudioSubsystem* Owner);

	void Play(USoundBase* Sound, FVector Location, USoundAttenuation* AttenuationOverride = nullptr, USoundConcurrency* ConcurrencyOverride = nullptr);
	UAudioComponent* PlayAttached(USoundBase* Sound, USceneComponent* Parent, FName SocketName = NAME_None, USoundAttenuation* AttenuationOverride = nullptr, USoundConcurrency* ConcurrencyOverride = nullptr);
	void Stop();

	UFUNCTION()
	void OnAudioFinished();

	void ValidateAudioComponent();

private:
	UPROPERTY()
	TObjectPtr<UAudioComponent> AudioComponent{ nullptr };

	UPROPERTY()
	TWeakObjectPtr<UGameplayAudioSubsystem> SubsystemOwner{ nullptr };
};
