// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.

#include "Audio/Environment/EnvironmentSoundEmitter.h"
#include "EnvironmentAudioSystem.h"
#include "EnvironmentAudioPlayerComponent.h"
#include "Components/SphereComponent.h"

AEnvironmentSoundEmitter::AEnvironmentSoundEmitter() {
	PrimaryActorTick.bCanEverTick = false;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootSceneComponent);

	TriggerRadiusPreviewSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerRadiusPreview"));
	TriggerRadiusPreviewSphere->SetupAttachment(RootSceneComponent);

	TriggerRadiusPreviewSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TriggerRadiusPreviewSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerRadiusPreviewSphere->SetGenerateOverlapEvents(false);
	TriggerRadiusPreviewSphere->SetCanEverAffectNavigation(false);
	TriggerRadiusPreviewSphere->SetHiddenInGame(true);

	UpdateTriggerRadiusPreview();
}

void AEnvironmentSoundEmitter::BeginPlay() {
	Super::BeginPlay();

	EnsureRuntimeState();

	if (UWorld* World = GetWorld()) {
		const float CurrentTime = World->GetTimeSeconds();
		const float FixedInitialDelayMax = FMath::Max(InitialDelayMin, InitialDelayMax);
		NextTriggerTime = CurrentTime + FMath::FRandRange(InitialDelayMin, InitialDelayMax);

		if (UEnvironmentAudioSystem* EnvironmentAudioSystem = World->GetSubsystem<UEnvironmentAudioSystem>()) {
			EnvironmentAudioSystem->RegisterSoundEmitter(this);
		}
	}
}

void AEnvironmentSoundEmitter::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	if (UWorld* World = GetWorld()) {
		if (UEnvironmentAudioSystem* EnvironmentAudioSystem = World->GetSubsystem<UEnvironmentAudioSystem>()) {
			EnvironmentAudioSystem->UnregisterSoundEmitter(this);
		}
	}

	Super::EndPlay(EndPlayReason);
}

#if WITH_EDITOR
void AEnvironmentSoundEmitter::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) {
	Super::PostEditChangeProperty(PropertyChangedEvent);

	UpdateTriggerRadiusPreview();
}
#endif

bool AEnvironmentSoundEmitter::TryTriggerForPlayer(UEnvironmentAudioPlayerComponent* PlayerComponent) {
	if (!PlayerComponent) return false;

	UWorld* World = GetWorld();
	if (!World) return false;

	const FVector PlayerLocation = PlayerComponent->GetPlayerLocation();
	const float DistanceSquaredToPlayer = FVector::DistSquared(PlayerLocation, GetActorLocation());
	if (DistanceSquaredToPlayer > FMath::Square(TriggerRadius)) return false;

	const bool bPlayerIdle = PlayerComponent->IsPlayerIdle();
	if (bPlayerIdle && !bCanTriggerWhilePlayerIdle) return false;
	if (bTriggerOnlyWhenPlayerMoves && bPlayerIdle) return false;

	const float CurrentTime = World->GetTimeSeconds();
	if (!CanTriggerNow(CurrentTime)) return false;

	// Schedule next attempt (even if chance might fail, else would instantly retry next scan
	ScheduleNextTriggerAttempt(CurrentTime);

	if (CurrentTime - LastGroupPlayTime < CooldownAfterSuccessfulPlay) return false;

	EnsureRuntimeState();

	const int32 SelectedEntryIndex = PickSoundEntryIndex(CurrentTime);
	if (!PossibleSounds.IsValidIndex(SelectedEntryIndex)) return false;

	const FEnvironmentRandomSoundEntry& SelectedEntry = PossibleSounds[SelectedEntryIndex];
	if (!SelectedEntry.IgnoresChance() && FMath::FRand() > SelectedEntry.Chance) return false;

	FVector PlayLocation = GetActorLocation();
	if (!ResolvePlayLocation(SelectedEntry, PlayerComponent, PlayLocation)) return false;

	UEnvironmentAudioSystem* EnvironmentAudioSystem = World->GetSubsystem<UEnvironmentAudioSystem>();
	if (!EnvironmentAudioSystem) return false;

	const bool bAccepted = EnvironmentAudioSystem->TryPlayRandomEnvironmentSound(PlayerComponent, SelectedEntry.SoundTag, PlayLocation, SelectedEntry.ForceMode);
	if (!bAccepted) return false;

	LastGroupPlayTime = CurrentTime;
	if (EntryRuntimeStates.IsValidIndex(SelectedEntryIndex)) {
		EntryRuntimeStates[SelectedEntryIndex].LastPlayedTime = CurrentTime;
		EntryRuntimeStates[SelectedEntryIndex].TimesPlayed++;
	}

	return true;
}

float AEnvironmentSoundEmitter::GetTriggerRadius() const {
	return TriggerRadius;
}

void AEnvironmentSoundEmitter::EnsureRuntimeState() {
	if (EntryRuntimeStates.Num() == PossibleSounds.Num()) {
		return;
	}

	EntryRuntimeStates.SetNum(PossibleSounds.Num());
}

bool AEnvironmentSoundEmitter::CanTriggerNow(float CurrentTime) const {
	return CurrentTime >= NextTriggerTime;
}

void AEnvironmentSoundEmitter::ScheduleNextTriggerAttempt(float CurrentTime) {
	const float MaxAttemptInterval = FMath::Max(TriggerIntervalMin, TriggerIntervalMax);
	const float NextInterval = FMath::FRandRange(TriggerIntervalMin, MaxAttemptInterval);
	NextTriggerTime = CurrentTime + NextInterval;
}

int32 AEnvironmentSoundEmitter::PickSoundEntryIndex(float CurrentTime) const {
	float TotalWeight = 0.0f;

	for (int32 EntryIndex = 0; EntryIndex < PossibleSounds.Num(); ++EntryIndex) {
		if (!IsSoundEntryCandidate(EntryIndex, CurrentTime)) continue;

		TotalWeight += PossibleSounds[EntryIndex].Weight;
	}

	if (TotalWeight <= 0.0f) {
		return INDEX_NONE;
	}

	float RandomWeight = FMath::FRandRange(0.0f, TotalWeight);

	for (int32 EntryIndex = 0; EntryIndex < PossibleSounds.Num(); ++EntryIndex) {
		if (!IsSoundEntryCandidate(EntryIndex, CurrentTime)) continue;

		RandomWeight -= PossibleSounds[EntryIndex].Weight;

		if (RandomWeight <= 0.0f) {
			return EntryIndex;
		}
	}

	return INDEX_NONE;
}

bool AEnvironmentSoundEmitter::IsSoundEntryCandidate(int32 EntryIndex, float CurrentTime) const {
	if (!PossibleSounds.IsValidIndex(EntryIndex)) return false;
	if (!EntryRuntimeStates.IsValidIndex(EntryIndex)) return false;

	const FEnvironmentRandomSoundEntry& Entry = PossibleSounds[EntryIndex];
	const FEnvironmentRandomSoundEntryRuntimeState& RuntimeState = EntryRuntimeStates[EntryIndex];

	if (!Entry.SoundTag.IsValid()) return false;
	if (Entry.Weight <= 0.0f) return false;

	if (Entry.MaxUses > 0 && RuntimeState.TimesPlayed >= Entry.MaxUses) {
		return false;
	}

	if (!Entry.IgnoresCooldown()) {
		const float TimeSinceLastPlay = CurrentTime - RuntimeState.LastPlayedTime;
		if (TimeSinceLastPlay < Entry.Cooldown) {
			return false;
		}
	}

	return true;
}

bool AEnvironmentSoundEmitter::ResolvePlayLocation(const FEnvironmentRandomSoundEntry& Entry, UEnvironmentAudioPlayerComponent* PlayerComponent, FVector& OutPlayLocation) const {
	if (!PlayerComponent) return false;

	switch (Entry.PlayLocationMode) {
	case EEnvironmentSoundPlayLocationMode::AtEmitter:
		OutPlayLocation = GetActorLocation();
		return true;

	case EEnvironmentSoundPlayLocationMode::AtPlayer:
		OutPlayLocation = PlayerComponent->GetPlayerLocation();
		return true;

	case EEnvironmentSoundPlayLocationMode::AroundPlayer:
		return PlayerComponent->DecideRandomLocationAroundPlayer(Entry.AroundPlayerLocationSettings, OutPlayLocation);

	case EEnvironmentSoundPlayLocationMode::AtLinkedActor:
		if (Entry.LinkedPlayActor) {
			OutPlayLocation = Entry.LinkedPlayActor->GetActorLocation();
			return true;
		}

		 // Play at the emitter location if no linked component is specified
		OutPlayLocation = GetActorLocation();
		return true;

	default:
		OutPlayLocation = GetActorLocation();
		return true;
	}
}

void AEnvironmentSoundEmitter::UpdateTriggerRadiusPreview() {
	if (!TriggerRadiusPreviewSphere) return;
	const float SafeRadius = FMath::Max(0.0f, TriggerRadius);
	TriggerRadiusPreviewSphere->SetSphereRadius(TriggerRadius, false);

	// Restore default in case they were changed
	TriggerRadiusPreviewSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TriggerRadiusPreviewSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerRadiusPreviewSphere->SetGenerateOverlapEvents(false);
	TriggerRadiusPreviewSphere->SetCanEverAffectNavigation(false);
	TriggerRadiusPreviewSphere->SetHiddenInGame(true);
}
