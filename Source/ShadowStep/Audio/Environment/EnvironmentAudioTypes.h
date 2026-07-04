// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.

#pragma once

#include "GameplayTagContainer.h"

#include "CoreMinimal.h"
#include "EnvironmentAudioTypes.generated.h"

class USceneComponent;

UENUM(BlueprintType)
enum class EEnvironmentSoundForceMode : uint8 {
	None UMETA(DisplayName = "None"),

	// Ignores the player's global random-environment sound rate limit.
	IgnoreGlobalRateLimit UMETA(DisplayName = "Ignore Global Rate Limit"),

	// Ignores the selected entry's chance roll and player's global rate limit.
	IgnoreChanceAndGlobalRateLimit UMETA(DisplayName = "Ignore Chance And Global Rate Limit"),

	// Ignores the selected entry's chance rool, entry cooldown, and player's global rate limit.
	// This does not ignore MaxUses
	IgnoreChanceCooldownAndGlobalRateLimit UMETA(DisplayName = "Ignore Chance, Cooldown And Global Rate Limit")
};

UENUM(BlueprintType)
enum class EEnvironmentSoundPlayLocationMode : uint8 {
	// Plays at the emitter actor's world location.
	AtEmitter UMETA(DisplayName = "At Emitter"),

	// Plays directly at the player's world location.
	AtPlayer UMETA(DisplayName = "At Player"),

	// Picks a random location around the player.
	AroundPlayer UMETA(DisplayName = "Around Player"),

	// Plays at a specific actor assigned on the entry.
	AtLinkedActor UMETA(DisplayName = "At Linked Actor")
};

UENUM(BlueprintType)
enum class EEnvironmentZoneLoopPlaybackMode : uint8 {
	// Non-spatial ambience. Best for room tone, outside wind
	TwoD UMETA(DisplayName = "2D"),

	// Spatial ambience placed at the zone actor's location
	AtZoneLocation UMETA(DisplayName = "At Zone Location"),

	// Spatial ambience placed at a linked actor, for example a fridge, generator, pipe, lamp, clock
	AtLinkedActor UMETA(DisplayName= "At Linked Actor")
};

/*
	Settings used when a sound should be placed randomly around the player.
	This is useful for sounds like: creaks around the player, movement behind player, bush rustling
*/
USTRUCT(BlueprintType)
struct FEnvironmentAroundPlayerLocationSettings {
	GENERATED_BODY()

	// Minimum horizontal distance
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0"), Category = "Around Player")
	float MinHorizontalDistanceOffset{ 300.0f };

	// Maximum horizontal distance
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0"), Category = "Around Player")
	float MaxHorizontalDistanceOffset{ 1200.0f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Around Player")
	float MinVerticalDistanceOffset{ -50.0f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Around Player")
	float MaxVerticalDistanceOffset{ 200.0f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Around Player")
	bool bTraceToGround{ true };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", EditCondition = "bTraceToGround", EditConditionHides), Category = "Around Player")
	float GroundTraceUpDistance{ 300.0f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", EditCondition = "bTraceToGround", EditConditionHides), Category = "Around Player")
	float GroundTraceDownDistance{ 800.0f };

	// If true, the selected point must be visible from the player
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Around Player|Line Of Sight")
	bool bRequireLineOfSight{ false };

	// If true, the selected point must be hidden from the player.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Around Player|Line Of Sight")
	bool bAvoidLineOfSight{ false };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0"), Category = "Around Player|Line Of Sight")
	float LineOfSightTargetHeightOffset{ 80.0f };

	// How many random points we try before giving up.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "1"), Category = "Around Player")
	int32 MaxLocationAttempts{ 8 };
};

USTRUCT(BlueprintType)
struct FEnvironmentRandomSoundEntry {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	FGameplayTag SoundTag{};

	// Selection weight.
	// 10 is ten times more likely to be selected than 1.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound", meta = (ClampMin = "0.0"))
	float Weight{ 1.0f };

	// Chance after the entry was selected.
	// 1.0 = always happens, 0.5 = 50%, 0.0 = never.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Chance{ 1.0f };

	// Cooldown for this specific entry
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cooldown", meta = (ClampMin = "0.0"))
	float Cooldown{ 0.0f };

	// Maximum number of times this entry can successfully play.
	// 0 means unlimited.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cooldown", meta = (ClampMin = "0"))
	int32 MaxUses{ 0 };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	EEnvironmentSoundForceMode ForceMode{ EEnvironmentSoundForceMode::None };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	EEnvironmentSoundPlayLocationMode PlayLocationMode{ EEnvironmentSoundPlayLocationMode::AtEmitter };

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta = (EditCondition = "PlayLocationMode == EEnvironmentSoundPlayLocationMode::AtLinkedActor", EditConditionHides), Category = "Location")
	TObjectPtr<AActor> LinkedPlayActor { nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "PlayLocationMode == EEnvironmentSoundPlayLocationMode::AroundPlayer", EditConditionHides), Category = "Location")
	FEnvironmentAroundPlayerLocationSettings AroundPlayerLocationSettings{};

	bool IgnoresChance() const {
		return ForceMode == EEnvironmentSoundForceMode::IgnoreChanceAndGlobalRateLimit
			|| ForceMode == EEnvironmentSoundForceMode::IgnoreChanceCooldownAndGlobalRateLimit;
	}

	bool IgnoresCooldown() const {
		return ForceMode == EEnvironmentSoundForceMode::IgnoreChanceCooldownAndGlobalRateLimit;
	}

	bool IgnoresGlobalRateLimit() const {
		return ForceMode != EEnvironmentSoundForceMode::None;
	}
};

/*
	Runtime state for one random sound entry.
*/
USTRUCT()
struct FEnvironmentRandomSoundEntryRuntimeState {
	GENERATED_BODY()

	float LastPlayedTime{ -1000000.0f };

	int32 TimesPlayed{ 0 };
};

USTRUCT(BlueprintType)
struct FEnvironmentZoneLoopSound {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	FGameplayTag SoundTag{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0"), Category = "Sound")
	float VolumeMultiplier{ 1.0f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.01"), Category = "Sound")
	float PitchMultiplier{ 1.0f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Playback")
	EEnvironmentZoneLoopPlaybackMode PlaybackMode{ EEnvironmentZoneLoopPlaybackMode::TwoD };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "PlaybackMode == EEnvironmentZoneLoopPlaybackMode::AtLinkedActor", EditConditionHides), Category = "Playback")
	TObjectPtr<AActor> LinkedPlayActor{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0"), Category = "Fade")
	float EnterFadeDuration{ 2.0f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0"), Category = "Fade")
	float ExitFadeDuration{ 2.0f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Playback")
	bool bRestartOnEnter{ false };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Playback")
	bool bStopAfterExitFade{ true };
};