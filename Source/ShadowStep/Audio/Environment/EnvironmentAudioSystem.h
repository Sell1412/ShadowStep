// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.

#pragma once

#include "Audio/Environment/EnvironmentAudioTypes.h"

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "EnvironmentAudioSystem.generated.h"

class AEnvironmentAudioZone;
class AEnvironmentSoundEmitter;
class UEnvironmentAudioPlayerComponent;

UCLASS()
class SHADOWSTEP_API UEnvironmentAudioSystem : public UWorldSubsystem {
	GENERATED_BODY()
	
public:
	virtual void Deinitialize() override;

	void RegisterPlayerComponent(UEnvironmentAudioPlayerComponent* PlayerComponent);
	void UnregisterPlayerComponent(UEnvironmentAudioPlayerComponent* PlayerComponent);

	void RegisterSoundEmitter(AEnvironmentSoundEmitter* SoundEmitter);
	void UnregisterSoundEmitter(AEnvironmentSoundEmitter* SoundEmiter);

	void RegisterAudioZone(AEnvironmentAudioZone* AudioZone);
	void UnregisterAudioZone(AEnvironmentAudioZone* AudioZone);

	// Called by 'UEnvironmentAudioPlayerComponent'
	void ScanEmittersForPlayer(UEnvironmentAudioPlayerComponent* PlayerComponent);
	void RefreshAudioZonesForPlayer(UEnvironmentAudioPlayerComponent* PlayerComponent);
	bool TryPlayRandomEnvironmentSound(UEnvironmentAudioPlayerComponent* PlayerComponent, FGameplayTag SoundTag, const FVector& PlayLocation, EEnvironmentSoundForceMode ForceMode);

private:
	void CompactInvalidReferences();

private:
	TArray<TWeakObjectPtr<UEnvironmentAudioPlayerComponent>> RegisteredPlayerComponents{};
	TArray<TWeakObjectPtr<AEnvironmentSoundEmitter>> RegisteredSoundEmitters{};
	TArray<TWeakObjectPtr<AEnvironmentAudioZone>> RegisteredAudioZones{};
};
