// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.

#include "Audio/Environment/EnvironmentAudioSystem.h"
#include "EnvironmentAudioPlayerComponent.h"
#include "EnvironmentSoundEmitter.h"
#include "Audio/GameplayAudioHelper.h"
#include "EnvironmentAudioZone.h"

void UEnvironmentAudioSystem::Deinitialize() {
	RegisteredPlayerComponents.Empty();
	RegisteredSoundEmitters.Empty();

	Super::Deinitialize();
}

void UEnvironmentAudioSystem::RegisterPlayerComponent(UEnvironmentAudioPlayerComponent* PlayerComponent) {
	if (!PlayerComponent) return;

	CompactInvalidReferences();
	for (const TWeakObjectPtr<UEnvironmentAudioPlayerComponent>& ExistingPlayerComponent : RegisteredPlayerComponents) {
		if (ExistingPlayerComponent.Get() == PlayerComponent) return;
	}

	RegisteredPlayerComponents.Add(PlayerComponent);
}

void UEnvironmentAudioSystem::UnregisterPlayerComponent(UEnvironmentAudioPlayerComponent* PlayerComponent) {
	if (!PlayerComponent) return;

	RegisteredPlayerComponents.RemoveAll(
		[PlayerComponent](const TWeakObjectPtr<UEnvironmentAudioPlayerComponent>& ExistingPlayerComponent) {
			return !ExistingPlayerComponent.IsValid() || ExistingPlayerComponent.Get() == PlayerComponent;
		}
	);
}

void UEnvironmentAudioSystem::RegisterSoundEmitter(AEnvironmentSoundEmitter* SoundEmitter) {
	if (!SoundEmitter) return;

	CompactInvalidReferences();
	for (const TWeakObjectPtr<AEnvironmentSoundEmitter>& ExistingSoundEmitter : RegisteredSoundEmitters) {
		if (ExistingSoundEmitter.Get() == SoundEmitter) return;
	}

	RegisteredSoundEmitters.Add(SoundEmitter);
}

void UEnvironmentAudioSystem::UnregisterSoundEmitter(AEnvironmentSoundEmitter* SoundEmiter) {
	if (!SoundEmiter) return;

	RegisteredSoundEmitters.RemoveAll(
		[SoundEmiter](const TWeakObjectPtr<AEnvironmentSoundEmitter>& ExistingSoundEmitter) {
			return !ExistingSoundEmitter.IsValid() || ExistingSoundEmitter.Get() == SoundEmiter;
		}
	);
}

void UEnvironmentAudioSystem::RegisterAudioZone(AEnvironmentAudioZone* AudioZone) {
	if (!AudioZone) return;

	CompactInvalidReferences();
	for (const TWeakObjectPtr<AEnvironmentAudioZone>& ExistingAudioZone : RegisteredAudioZones) {
		if (ExistingAudioZone.Get() == AudioZone) return;
	}

	RegisteredAudioZones.Add(AudioZone);
}

void UEnvironmentAudioSystem::UnregisterAudioZone(AEnvironmentAudioZone* AudioZone) {
	if (!AudioZone) return;

	RegisteredAudioZones.RemoveAll(
		[AudioZone](const TWeakObjectPtr<AEnvironmentAudioZone>& ExistingAudioZone) {
			return !ExistingAudioZone.IsValid() || ExistingAudioZone.Get() == AudioZone;
		}
	);
}

void UEnvironmentAudioSystem::ScanEmittersForPlayer(UEnvironmentAudioPlayerComponent* PlayerComponent) {
	if (!PlayerComponent) return;

	CompactInvalidReferences();

	for (const TWeakObjectPtr<AEnvironmentSoundEmitter>& SoundEmitterPtr : RegisteredSoundEmitters) {
		AEnvironmentSoundEmitter* SoundEmitter = SoundEmitterPtr.Get();
		if (!SoundEmitter) continue;

		SoundEmitter->TryTriggerForPlayer(PlayerComponent);
	}
}

void UEnvironmentAudioSystem::RefreshAudioZonesForPlayer(UEnvironmentAudioPlayerComponent* PlayerComponent) {
	if (!PlayerComponent) return;

	CompactInvalidReferences();

	TArray<AEnvironmentAudioZone*> CurrentlyContainingZones{};
	const FVector PlayerLocation = PlayerComponent->GetPlayerLocation();

	for (const TWeakObjectPtr<AEnvironmentAudioZone>& AudioZonePointer : RegisteredAudioZones) {
		AEnvironmentAudioZone* AudioZone = AudioZonePointer.Get();
		if (!AudioZone) continue;

		if (AudioZone->ContainsWorldLocation(PlayerLocation)) {
			CurrentlyContainingZones.Add(AudioZone);
		}
	}

	PlayerComponent->UpdateAudioZoneInclusion(CurrentlyContainingZones);
}

bool UEnvironmentAudioSystem::TryPlayRandomEnvironmentSound(UEnvironmentAudioPlayerComponent* PlayerComponent, FGameplayTag SoundTag, const FVector& PlayLocation, EEnvironmentSoundForceMode ForceMode) {
	if (!PlayerComponent) return false;
	if (!SoundTag.IsValid()) return false;

	if (!PlayerComponent->CanPlayRandomEnvironmentSound(ForceMode)) {
		return false;
	}

	UGameplayAudioHelper::PlaySoundAtLocation(PlayerComponent, SoundTag, PlayLocation);
	PlayerComponent->NotifyRandomEnvironmentSoundPlayed();

	return true;
}

void UEnvironmentAudioSystem::CompactInvalidReferences() {
	RegisteredPlayerComponents.RemoveAll(
		[](const TWeakObjectPtr<UEnvironmentAudioPlayerComponent>& PlayerComponent) {
			return !PlayerComponent.IsValid();
		}
	);

	RegisteredSoundEmitters.RemoveAll(
		[](const TWeakObjectPtr<AEnvironmentSoundEmitter>& SoundEmitter) {
			return !SoundEmitter.IsValid();
		}
	);

	RegisteredAudioZones.RemoveAll(
		[](const TWeakObjectPtr<AEnvironmentAudioZone>& AudioZone) {
			return !AudioZone.IsValid();
		}
	);
}
