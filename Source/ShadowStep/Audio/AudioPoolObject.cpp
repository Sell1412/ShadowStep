// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.

#include "Audio/AudioPoolObject.h"

#include "Components/AudioComponent.h"
#include "GameplayAudioSubsystem.h"
#include "Sound/SoundAttenuation.h"
#include "Sound/SoundConcurrency.h"
#include "Sound/SoundBase.h"

void UAudioPoolObject::Init(UGameplayAudioSubsystem* Owner) {
	SubsystemOwner = Owner;

	ValidateAudioComponent(); // Pre-warms the pool by creating the audio components
}

void UAudioPoolObject::Play(USoundBase* Sound, FVector Location, USoundAttenuation* AttenuationOverride /*= nullptr*/, USoundConcurrency* ConcurrencyOverride /*= nullptr*/) {
	ValidateAudioComponent();

	if (!AudioComponent || !Sound) {
		OnAudioFinished(); return;
	}

	UWorld* CurrentWorld = SubsystemOwner.IsValid() ? SubsystemOwner->GetWorld() : nullptr;
	if (!CurrentWorld) {
		OnAudioFinished(); return;
	}

	if (!AudioComponent->IsRegistered())
		AudioComponent->RegisterComponentWithWorld(CurrentWorld);

	AudioComponent->Stop(); // Just in case

	// Detach if was previously attached
	AudioComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

	AudioComponent->SetSound(Sound);
	AudioComponent->SetWorldLocation(Location);

	// Apply overrides
	AudioComponent->AttenuationSettings = AttenuationOverride; // Cleared if null

	if (ConcurrencyOverride) {
		AudioComponent->ConcurrencySet.Empty(1);
		AudioComponent->ConcurrencySet.Add(ConcurrencyOverride);
	} 
	else
		AudioComponent->ConcurrencySet.Empty();

	AudioComponent->Play();
}

UAudioComponent* UAudioPoolObject::PlayAttached(USoundBase* Sound, USceneComponent* Parent, FName SocketName /*= NAME_None*/, USoundAttenuation* AttenuationOverride /*= nullptr*/, USoundConcurrency* ConcurrencyOverride /*= nullptr*/) {
	ValidateAudioComponent();
	
	if (!Parent || !AudioComponent || !Sound) {
		OnAudioFinished(); return nullptr;
	}

	UWorld* CurrentWorld = SubsystemOwner.IsValid() ? SubsystemOwner->GetWorld() : nullptr;
	if (!CurrentWorld) {
		OnAudioFinished(); return nullptr;
	}

	if (!AudioComponent->IsRegistered())
		AudioComponent->RegisterComponentWithWorld(CurrentWorld);

	AudioComponent->Stop();

	AudioComponent->SetSound(Sound);

	// Apply overrides
	AudioComponent->AttenuationSettings = AttenuationOverride;

	if (ConcurrencyOverride) {
		AudioComponent->ConcurrencySet.Empty(1);
		AudioComponent->ConcurrencySet.Add(ConcurrencyOverride);
	}
	else
		AudioComponent->ConcurrencySet.Empty();

	// NOTE: If no socket is specified, it snaps to the component's pivot
	AudioComponent->AttachToComponent(Parent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);

	AudioComponent->Play();
	return AudioComponent;
}

void UAudioPoolObject::Stop() {
	if (AudioComponent) AudioComponent->Stop(); // Automatically calls 'OnAudioFinished'
}

void UAudioPoolObject::OnAudioFinished() {
	if (AudioComponent) AudioComponent->SetSound(nullptr); // Clear reference -> allows GC if stack is unloaded
	if (SubsystemOwner.IsValid())
		SubsystemOwner->ReturnToPool(this);
}

void UAudioPoolObject::ValidateAudioComponent() {
	if (!SubsystemOwner.IsValid()) return;
	UWorld* CurrentWorld = SubsystemOwner->GetWorld();
	if (!CurrentWorld) return;

	// If the component belongs to a different world, we destroy and recreate it for safety
	if (AudioComponent && AudioComponent->GetWorld() != CurrentWorld) {
		if (AudioComponent->OnAudioFinished.IsAlreadyBound(this, &UAudioPoolObject::OnAudioFinished))
			AudioComponent->OnAudioFinished.RemoveDynamic(this, &UAudioPoolObject::OnAudioFinished);

		AudioComponent->DestroyComponent();
		AudioComponent = nullptr;
	}

	if (!AudioComponent) {
		// Use the game instance as the outer, so the object can survive across levels -> needed for: AudioComponent->GetWorld() != currentWorld
		AudioComponent = NewObject<UAudioComponent>(SubsystemOwner.Get());
		AudioComponent->bAutoActivate = false;
		AudioComponent->bAutoDestroy = false;
		AudioComponent->bAutoManageAttachment = false;
		AudioComponent->SetMobility(EComponentMobility::Movable);
		AudioComponent->OnAudioFinished.AddDynamic(this, &UAudioPoolObject::OnAudioFinished);

		// NOTE: Don't register here -> world might not fully exist yet -> will be handled in Play()
	}
}
