// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.

#include "Audio/Environment/EnvironmentAudioZone.h"
#include "EnvironmentAudioPlayerComponent.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "EnvironmentAudioSystem.h"
#include "Audio/GameplayAudioHelper.h"

AEnvironmentAudioZone::AEnvironmentAudioZone() {
	PrimaryActorTick.bCanEverTick = false;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootSceneComponent);

	ZoneBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("ZoneBounds"));
	ZoneBounds->SetupAttachment(RootSceneComponent);

	// Is only for designers to see the size, not for actual collision detection.
	ZoneBounds->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ZoneBounds->SetCollisionResponseToAllChannels(ECR_Ignore);
	ZoneBounds->SetGenerateOverlapEvents(false);
	ZoneBounds->SetCanEverAffectNavigation(false);
	ZoneBounds->SetHiddenInGame(true);

	ZoneBounds->SetBoxExtent(FVector(500.0f, 500.0f, 250.0f));
}

void AEnvironmentAudioZone::BeginPlay() {
	Super::BeginPlay();

	CreateAudioComponentsIfNeeded();

	if (UWorld* World = GetWorld()) {
		if (UEnvironmentAudioSystem* EnvironmentAudioSystem = World->GetSubsystem<UEnvironmentAudioSystem>()) {
			EnvironmentAudioSystem->RegisterAudioZone(this);
		}
	}
}

void AEnvironmentAudioZone::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	if (UWorld* World = GetWorld()) {
		if (UEnvironmentAudioSystem* EnvironmentAudioSystem = World->GetSubsystem<UEnvironmentAudioSystem>()) {
			EnvironmentAudioSystem->UnregisterAudioZone(this);
		}
	}

	for (auto& LoopAudioComponentPair : LoopAudioComponents) {
		UAudioComponent* LoopAudioComponent = LoopAudioComponentPair.Value;
		if (!LoopAudioComponent) continue;

		LoopAudioComponent->Stop();
		LoopAudioComponent->DestroyComponent();
	}

	LoopAudioComponents.Empty();

	Super::EndPlay(EndPlayReason);
}

bool AEnvironmentAudioZone::ContainsWorldLocation(const FVector& WorldLocation) const {
	if (!ZoneBounds) return false;

	// Convert to local space -> supports rotation/scaling
	const FVector LocalLocation = ZoneBounds->GetComponentTransform().InverseTransformPosition(WorldLocation);
	const FVector BoxExtent = ZoneBounds->GetUnscaledBoxExtent();

	return FMath::Abs(LocalLocation.X) <= BoxExtent.X && FMath::Abs(LocalLocation.Y) <= BoxExtent.Y && FMath::Abs(LocalLocation.Z) <= BoxExtent.Z;
}

void AEnvironmentAudioZone::HandlePlayerEnteredZone(UEnvironmentAudioPlayerComponent* PlayerComponent) {
	if (!PlayerComponent) return;

	CreateAudioComponentsIfNeeded();

	for (int32 LoopIndex = 0; LoopIndex < LoopSounds.Num(); ++LoopIndex) {
		FadeInLoopSound(LoopIndex, PlayerComponent);
	}

	OnPlayerEntered_BP.Broadcast(PlayerComponent);
}

void AEnvironmentAudioZone::HandlePlayerExitedZone(UEnvironmentAudioPlayerComponent* PlayerComponent) {
	for (int32 LoopIndex = 0; LoopIndex < LoopSounds.Num(); ++LoopIndex) {
		FadeOutLoopSound(LoopIndex);
	}

	OnPlayerExited_BP.Broadcast(PlayerComponent);
}

void AEnvironmentAudioZone::CreateAudioComponentsIfNeeded() {
	for (auto It = LoopAudioComponents.CreateIterator(); It; ++It) {
		const int32 LoopIndex = It.Key();
		UAudioComponent* LoopAudioComponent = It.Value();

		const bool bLoopIndexStillExists = LoopSounds.IsValidIndex(LoopIndex);
		const bool bComponentStillValid = IsValid(LoopAudioComponent);

		if (bLoopIndexStillExists && bComponentStillValid) continue;

		if (bComponentStillValid) {
			LoopAudioComponent->Stop();
			LoopAudioComponent->DestroyComponent();
		}

		It.RemoveCurrent();
	}

	for (int32 LoopIndex = 0; LoopIndex < LoopSounds.Num(); ++LoopIndex) {
		GetOrCreateAudioComponent(LoopIndex);
	}
}

UAudioComponent* AEnvironmentAudioZone::GetOrCreateAudioComponent(int32 LoopIndex) {
	if (!LoopSounds.IsValidIndex(LoopIndex)) return nullptr;

	if (TObjectPtr<UAudioComponent>* ExistingComponentPointer = LoopAudioComponents.Find(LoopIndex)) {
		UAudioComponent* ExistingComponent = ExistingComponentPointer ? ExistingComponentPointer-> Get() : nullptr;

		if (IsValid(ExistingComponent)) {
			return ExistingComponent;
		}

		// Component no longer valid -> creates a new one
		LoopAudioComponents.Remove(LoopIndex);
	}

	UAudioComponent* NewAudioComponent = NewObject<UAudioComponent>(this);
	if (!NewAudioComponent) return nullptr;

	NewAudioComponent->bAutoActivate = false;
	NewAudioComponent->bAutoDestroy = false;
	NewAudioComponent->bAutoManageAttachment = false;
	NewAudioComponent->SetMobility(EComponentMobility::Movable);

	if (UWorld* World = GetWorld()) {
		NewAudioComponent->RegisterComponentWithWorld(World);
	}

	LoopAudioComponents.Add(LoopIndex, NewAudioComponent);
	return NewAudioComponent;
}

void AEnvironmentAudioZone::FadeInLoopSound(uint32 LoopIndex, UEnvironmentAudioPlayerComponent* PlayerComponent) {
	if (!LoopSounds.IsValidIndex(LoopIndex)) return;
	const FEnvironmentZoneLoopSound& LoopSound = LoopSounds[LoopIndex];
	if (!LoopSound.SoundTag.IsValid()) return;
	UAudioComponent* LoopAudioComponent = GetOrCreateAudioComponent(LoopIndex);
	if (!LoopAudioComponent) return;
	const FVector PlayLocation = ResolveLoopWorldLocation(LoopSound, PlayerComponent);
	if (LoopSound.bRestartOnEnter) LoopAudioComponent->Stop();

	switch (LoopSound.PlaybackMode) {
	case EEnvironmentZoneLoopPlaybackMode::TwoD:
		UGameplayAudioHelper::PlaySound2DWithExistingComponent(LoopAudioComponent, LoopSound.SoundTag, LoopSound.VolumeMultiplier, LoopSound.PitchMultiplier, false);
		break;
	case EEnvironmentZoneLoopPlaybackMode::AtZoneLocation:
	case EEnvironmentZoneLoopPlaybackMode::AtLinkedActor:
		UGameplayAudioHelper::PlaySoundAtLocationWithExistingComponent(LoopAudioComponent, LoopSound.SoundTag, PlayLocation);
		LoopAudioComponent->SetVolumeMultiplier(LoopSound.VolumeMultiplier);
		LoopAudioComponent->SetPitchMultiplier(LoopSound.PitchMultiplier);
		break;
	default:
		break;
	}

	LoopAudioComponent->FadeIn(LoopSound.EnterFadeDuration, LoopSound.VolumeMultiplier, 0.0f);
}

void AEnvironmentAudioZone::FadeOutLoopSound(uint32 LoopIndex) {
	if (!LoopSounds.IsValidIndex(LoopIndex)) return;

	const FEnvironmentZoneLoopSound& LoopSound = LoopSounds[LoopIndex];
	UAudioComponent* LoopAudioComponent = GetOrCreateAudioComponent(LoopIndex);
	if (!LoopAudioComponent) return;

	if (!LoopAudioComponent->IsPlaying()) return;

	if (LoopSound.bStopAfterExitFade) {
		LoopAudioComponent->FadeOut(LoopSound.ExitFadeDuration, 0.0f);
	}
	else {
		// Useful for ambience that runs silently (helps avoid sounding repetitive)
		LoopAudioComponent->AdjustVolume(LoopSound.ExitFadeDuration, 0.0f);
	}
}

FVector AEnvironmentAudioZone::ResolveLoopWorldLocation(const FEnvironmentZoneLoopSound& LoopSound, UEnvironmentAudioPlayerComponent* PlayerComponent) const {
	switch (LoopSound.PlaybackMode) {
	case EEnvironmentZoneLoopPlaybackMode::AtLinkedActor:
		if (LoopSound.LinkedPlayActor) return LoopSound.LinkedPlayActor->GetActorLocation();

		return GetActorLocation();
	case EEnvironmentZoneLoopPlaybackMode::AtZoneLocation:
		return GetActorLocation();
	case EEnvironmentZoneLoopPlaybackMode::TwoD:
	default:
		return FVector::ZeroVector;
	}
}
