// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.

#include "Audio/Environment/EnvironmentAudioPlayerComponent.h"
#include "EnvironmentAudioSystem.h"
#include "EnvironmentAudioZone.h"

UEnvironmentAudioPlayerComponent::UEnvironmentAudioPlayerComponent() {
	PrimaryComponentTick.bCanEverTick = true;
}

void UEnvironmentAudioPlayerComponent::BeginPlay() {
	Super::BeginPlay();

	if (AActor* Owner = GetOwner()) {
		PreviousOwnerLocation = Owner->GetActorLocation();
	}

	if (UEnvironmentAudioSystem* EnvironmentAudioSystem = GetEnvironmentAudioSystem()) {
		EnvironmentAudioSystem->RegisterPlayerComponent(this);
	}

	if (UWorld* World = GetWorld()) {
		const float CurrentTime = World->GetTimeSeconds();
		NextEmitterScanTime = CurrentTime + EmitterScanInterval;

		// Scan the zones instantly at the beginning
		NextZoneScanTime = CurrentTime;
	}
}

void UEnvironmentAudioPlayerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	// Fade out all currently active zones
	for (const TWeakObjectPtr<AEnvironmentAudioZone>& ActiveZonePointer : ActiveAudioZones) {
		if (AEnvironmentAudioZone* ActiveZone = ActiveZonePointer.Get()) {
			ActiveZone->HandlePlayerExitedZone(this);
		}
	}

	ActiveAudioZones.Empty();

	if (UEnvironmentAudioSystem* EnvironmentAudioSystem = GetEnvironmentAudioSystem()) {
		EnvironmentAudioSystem->UnregisterPlayerComponent(this);
	}

	Super::EndPlay(EndPlayReason);
}

void UEnvironmentAudioPlayerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateIdleState(DeltaTime);
	UpdateEmitterScanning();
	UpdateAudioZoneScanning();
}

bool UEnvironmentAudioPlayerComponent::CanPlayRandomEnvironmentSound(EEnvironmentSoundForceMode ForceMode) {
	const bool bIgnoreGlobalRateLimit = ForceMode != EEnvironmentSoundForceMode::None;
	if (bIgnoreGlobalRateLimit) return true;

	UWorld* World = GetWorld();
	if (!World) return false;

	const float CurrentTime = World->GetTimeSeconds();
	if (CurrentTime - LastRandomEnvironmentSoundTime < MinimumSecondsBetweenRandomEnvironmentSounds) {
		return false;
	}

	RemoveSoundsOlderThanASecond(CurrentTime);

	if (MaximumRandomEnvironmentSoundsPerSecond > 0 && RecentRandomEnvironmentSoundTimes.Num() >= MaximumRandomEnvironmentSoundsPerSecond) {
		return false;
	}

	return true;
}

void UEnvironmentAudioPlayerComponent::NotifyRandomEnvironmentSoundPlayed() {
	UWorld* World = GetWorld();
	if (!World) return;

	const float CurrentTime = World->GetTimeSeconds();
	LastRandomEnvironmentSoundTime = CurrentTime;

	RemoveSoundsOlderThanASecond(CurrentTime);
	RecentRandomEnvironmentSoundTimes.Add(CurrentTime);
}

bool UEnvironmentAudioPlayerComponent::IsPlayerIdle() const {
	return bIsPlayerIdle;
}

FVector UEnvironmentAudioPlayerComponent::GetPlayerLocation() const {
	return GetComponentLocation();
}

bool UEnvironmentAudioPlayerComponent::DecideRandomLocationAroundPlayer(const FEnvironmentAroundPlayerLocationSettings& Settings, FVector& OutLocation) const {
	const AActor* Owner = GetOwner();
	UWorld* World = GetWorld();

	if (!Owner || !World) return false;

	const FVector PlayerLocation = GetComponentLocation();
	const float MinHorizontalDistance = FMath::Max(0.0f, Settings.MinHorizontalDistanceOffset);
	const float MaxHorizontalDistance = FMath::Max(MinHorizontalDistance, Settings.MaxHorizontalDistanceOffset);
	const int32 MaxAttempts = FMath::Max(1, Settings.MaxLocationAttempts);

	FCollisionQueryParams CollisionsParams;
	CollisionsParams.AddIgnoredActor(Owner);

	for (int32 AttemptIndex = 0; AttemptIndex < MaxAttempts; ++AttemptIndex) {
		const float RandomAngle = FMath::FRandRange(0.0f, 2.0f * PI);
		const FVector RandomDirection = FVector(FMath::Cos(RandomAngle), FMath::Sin(RandomAngle), 0.0f);

		const float RandomDistance = FMath::FRandRange(MinHorizontalDistance, MaxHorizontalDistance);
		const float RandomVerticalOffset = FMath::FRandRange(Settings.MinVerticalDistanceOffset, Settings.MaxVerticalDistanceOffset);

		FVector CandidateLocation = PlayerLocation + (RandomDirection * RandomDistance);
		CandidateLocation.Z += RandomVerticalOffset;

		if (Settings.bTraceToGround) {
			const FVector TraceStart = CandidateLocation + FVector(0.0f, 0.0f, Settings.GroundTraceUpDistance);
			const FVector TraceEnd = CandidateLocation - FVector(0.0f, 0.0f, Settings.GroundTraceDownDistance);

			FHitResult GroundHit;
			const bool bHitGround = World->LineTraceSingleByChannel(GroundHit, TraceStart, TraceEnd, ECC_Visibility, CollisionsParams);

			if (bHitGround) CandidateLocation = GroundHit.ImpactPoint;
		}

		const bool bNeedsLineOfSightCheck = Settings.bRequireLineOfSight || Settings.bAvoidLineOfSight;
		if (bNeedsLineOfSightCheck) {
			const FVector LineOfSightStart = PlayerLocation;
			const FVector LineOfSightEnd = CandidateLocation + FVector(0.0f, 0.0f, Settings.LineOfSightTargetHeightOffset);

			FHitResult LineOfSightHit;
			const bool bLineOfSightBlocked = World->LineTraceSingleByChannel(LineOfSightHit, LineOfSightStart, LineOfSightEnd, ECC_Visibility, CollisionsParams);

			if (Settings.bRequireLineOfSight && bLineOfSightBlocked) {
				continue;
			}

			if (Settings.bAvoidLineOfSight && !bLineOfSightBlocked) {
				continue;
			}
		}

		OutLocation = CandidateLocation;
		return true;
	}

	return false;
}

void UEnvironmentAudioPlayerComponent::UpdateAudioZoneInclusion(const TArray<AEnvironmentAudioZone*>& CurrentlyContainingZones) {
	RemoveInvalidActiveZones();

	// Enter new zone
	for (AEnvironmentAudioZone* CurrentZone : CurrentlyContainingZones) {
		if (!CurrentZone) continue;
		if (IsZoneCurrentlyActive(CurrentZone)) continue;

		ActiveAudioZones.Add(CurrentZone);
		CurrentZone->HandlePlayerEnteredZone(this);
	}

	// Exit zones that were active but no longer contain the player
	for (int32 ActiveZoneIndex = ActiveAudioZones.Num() - 1; ActiveZoneIndex >= 0; --ActiveZoneIndex) {
		AEnvironmentAudioZone* ActiveZone = ActiveAudioZones[ActiveZoneIndex].Get();

		if (!ActiveZone) {
			ActiveAudioZones.RemoveAt(ActiveZoneIndex); continue;
		}

		if (!CurrentlyContainingZones.Contains(ActiveZone)) {
			ActiveZone->HandlePlayerExitedZone(this);
			ActiveAudioZones.RemoveAt(ActiveZoneIndex);
		}
	}
}

void UEnvironmentAudioPlayerComponent::UpdateIdleState(float DeltaTime) {
	AActor* Owner = GetOwner();
	if (!Owner || DeltaTime <= 0.0f) return;

	const FVector CurrentOwnerLocation = Owner->GetActorLocation();
	const float DistanceThisFrame = FVector::Dist(CurrentOwnerLocation, PreviousOwnerLocation);
	const float CurrentSpeed = DistanceThisFrame / DeltaTime;

	PreviousOwnerLocation = CurrentOwnerLocation;
	if (CurrentSpeed > IdleMovementSpeedThreshold) {
		TimeSinceMeaningfulMovement = 0.0f;
		bIsPlayerIdle = false;
		return;
	}

	TimeSinceMeaningfulMovement += DeltaTime;
	bIsPlayerIdle = TimeSinceMeaningfulMovement >= TimeBeforeConsideredIdle;
}

void UEnvironmentAudioPlayerComponent::UpdateEmitterScanning() {
	UWorld* World = GetWorld();
	if (!World) return;

	const float CurrentTime = World->GetTimeSeconds();
	if (CurrentTime < NextEmitterScanTime) return;

	NextEmitterScanTime = CurrentTime + EmitterScanInterval;
	if (UEnvironmentAudioSystem* EnvironmentAudioSystem = GetEnvironmentAudioSystem()) {
		EnvironmentAudioSystem->ScanEmittersForPlayer(this);
	}
}

void UEnvironmentAudioPlayerComponent::UpdateAudioZoneScanning() {
	UWorld* World = GetWorld();
	if (!World) return;

	const float CurrentTime = World->GetTimeSeconds();
	if (CurrentTime < NextZoneScanTime) return;

	NextZoneScanTime = CurrentTime + ZoneScanInterval;
	if (UEnvironmentAudioSystem* EnvironmentAudioSystem = GetEnvironmentAudioSystem()) {
		EnvironmentAudioSystem->RefreshAudioZonesForPlayer(this);
	}
}

UEnvironmentAudioSystem* UEnvironmentAudioPlayerComponent::GetEnvironmentAudioSystem() const {
	UWorld* World = GetWorld();
	if (!World) return nullptr;

	return World->GetSubsystem<UEnvironmentAudioSystem>();
}

void UEnvironmentAudioPlayerComponent::RemoveSoundsOlderThanASecond(float CurrentTime) {
	RecentRandomEnvironmentSoundTimes.RemoveAll(
		[CurrentTime](float SoundTime) {
			return CurrentTime - SoundTime > 1.0f; // Remove all sounds older than a second
		}
	);
}

bool UEnvironmentAudioPlayerComponent::IsZoneCurrentlyActive(AEnvironmentAudioZone* AudioZone) const {
	if (!AudioZone) return false;

	for (const TWeakObjectPtr<AEnvironmentAudioZone>& ActiveZonePointer : ActiveAudioZones) {
		if (ActiveZonePointer.Get() == AudioZone) return true;
	}

	return false;
}

void UEnvironmentAudioPlayerComponent::RemoveInvalidActiveZones() {
	ActiveAudioZones.RemoveAll(
		[](const TWeakObjectPtr<AEnvironmentAudioZone>& ActiveZonePointer) {
			return !ActiveZonePointer.IsValid();
		}
	);
}

