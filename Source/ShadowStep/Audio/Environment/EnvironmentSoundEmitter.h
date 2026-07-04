// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.

#pragma once

#include "Audio/Environment/EnvironmentAudioTypes.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnvironmentSoundEmitter.generated.h"

class UEnvironmentAudioPlayerComponent;
class USphereComponent;

/*
	If a 'UEnvironmentAudioPlayerComponent' is close enough, this emitter occasionally attempts a sound.
*/
UCLASS()
class SHADOWSTEP_API AEnvironmentSoundEmitter : public AActor {
	GENERATED_BODY()
	
public:	
	AEnvironmentSoundEmitter();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:	
	bool TryTriggerForPlayer(UEnvironmentAudioPlayerComponent* PlayerComponent);
	float GetTriggerRadius() const;

private:
	void EnsureRuntimeState();
	bool CanTriggerNow(float CurrentTime) const;
	void ScheduleNextTriggerAttempt(float CurrentTime);

	int32 PickSoundEntryIndex(float CurrentTime) const;
	bool IsSoundEntryCandidate(int32 EntryIndex, float CurrentTime) const;

	bool ResolvePlayLocation(const FEnvironmentRandomSoundEntry& Entry, UEnvironmentAudioPlayerComponent* PlayerComponent, FVector& OutPlayLocation) const;

	void UpdateTriggerRadiusPreview();

private:
	UPROPERTY(VisibleAnywhere, Category="Own|Environment Audio")
	TObjectPtr<USceneComponent> RootSceneComponent{ nullptr };

	UPROPERTY(VisibleAnywhere, Category="Own|Environment Audio|Debug")
	TObjectPtr<USphereComponent> TriggerRadiusPreviewSphere{ nullptr };

	// Player must be within this radius for the emitter to attempt sounds.
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"), Category = "Own|Environment Audio|Trigger")
	float TriggerRadius{ 1200.0f };

	UPROPERTY(EditAnywhere, Category="Own|Environment Audio|Trigger")
	bool bCanTriggerWhilePlayerIdle{ true };

	// Useful for bushes/grass/rattles that should react to movement.
	UPROPERTY(EditAnywhere, Category="Own|Environment Audio|Trigger")
	bool bTriggerOnlyWhenPlayerMoves{};

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"), Category = "Own|Environment Audio|Timing")
	float TriggerIntervalMin{ 4.0f };

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"), Category = "Own|Environment Audio|Timing")
	float TriggerIntervalMax{ 8.0f };

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"), Category = "Own|Environment Audio|Timing")
	float InitialDelayMin{ 0.0f };

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"), Category = "Own|Environment Audio|Timing")
	float InitialDelayMax{ 2.0f };

	// Cooldown for this whole emitter after any sound successfully plays.
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"), Category = "Own|Environment Audio|Cooldown")
	float CooldownAfterSuccessfulPlay{ 3.0f };

	UPROPERTY(EditAnywhere, Category="Own|Environment Audio|Sounds")
	TArray<FEnvironmentRandomSoundEntry> PossibleSounds{};

private:
	TArray<FEnvironmentRandomSoundEntryRuntimeState> EntryRuntimeStates{};
	float NextTriggerTime{ 0.0f };
	float LastGroupPlayTime{ -1000000.0f };

};
