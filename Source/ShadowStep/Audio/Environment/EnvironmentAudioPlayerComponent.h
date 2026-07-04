// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.

#pragma once

#include "Audio/Environment/EnvironmentAudioTypes.h"

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "EnvironmentAudioPlayerComponent.generated.h"

class AEnvironmentAudioZone;
class UEnvironmentAudioSystem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHADOWSTEP_API UEnvironmentAudioPlayerComponent : public USceneComponent {
	GENERATED_BODY()

public:	
	UEnvironmentAudioPlayerComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:	
	bool CanPlayRandomEnvironmentSound(EEnvironmentSoundForceMode ForceMode);
	void NotifyRandomEnvironmentSoundPlayed();

	bool IsPlayerIdle() const;
	FVector GetPlayerLocation() const;

	bool DecideRandomLocationAroundPlayer(const FEnvironmentAroundPlayerLocationSettings& Settings, FVector& OutLocation) const;
	void UpdateAudioZoneInclusion(const TArray<AEnvironmentAudioZone*>& CurrentlyContainingZones);
	
private:
	void UpdateIdleState(float DeltaTime);
	void UpdateEmitterScanning();
	void UpdateAudioZoneScanning();

	UEnvironmentAudioSystem* GetEnvironmentAudioSystem() const;
	void RemoveSoundsOlderThanASecond(float CurrentTime);

	bool IsZoneCurrentlyActive(AEnvironmentAudioZone* AudioZone) const;
	void RemoveInvalidActiveZones();

private:
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.01"), Category = "Own|Environment Audio|Scanning")
	float EmitterScanInterval{ 0.25f };

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.01"), Category = "Own|Environment Audio|Scanning")
	float ZoneScanInterval{ 0.15f };

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"), Category = "Own|Environment Audio|Rate Limit")
	float MinimumSecondsBetweenRandomEnvironmentSounds{ 1.0f };

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0"), Category = "Own|Environment Audio|Rate Limit")
	int32 MaximumRandomEnvironmentSoundsPerSecond{ 2 };

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"), Category = "Own|Environment Audio|Idle")
	float IdleMovementSpeedThreshold{ 5.0f };

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"), Category = "Own|Environment Audio|Idle")
	float TimeBeforeConsideredIdle{ 5.0f };

private:
	FVector PreviousOwnerLocation{};
	float TimeSinceMeaningfulMovement{};
	bool bIsPlayerIdle{};

	float NextEmitterScanTime{};
	float NextZoneScanTime{};

	float LastRandomEnvironmentSoundTime{ -1000000.0f };

	TArray<float> RecentRandomEnvironmentSoundTimes{};
	TArray<TWeakObjectPtr<AEnvironmentAudioZone>> ActiveAudioZones{};
};
