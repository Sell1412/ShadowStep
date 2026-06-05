// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SettingsSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class SHADOWSTEP_API USettingsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	// Setters
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetMasterVolume(float Volume) { m_MasterVolume = Volume; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetMusicVolume(float Volume) { m_MusicVolume = Volume; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetEffectsVolume(float Volume) { m_EffectsVolume = Volume; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetMasterMuted(bool bMuted) { m_MasterMuted = bMuted; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetMusicMuted(bool bMuted) { m_MusicMuted = bMuted; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetEffectsMuted(bool bMuted) { m_EffectsMuted = bMuted; }


	// Getters

	UFUNCTION(BlueprintCallable, Category = "Settings")
	float GetMasterVolume() const { return m_MasterVolume; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
	float GetMusicVolume() const { return m_MusicVolume; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
	float GetEffectsVolume() const { return m_EffectsVolume; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
	bool IsMasterMuted() const { return m_MasterMuted; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
	bool IsMusicMuted() const { return m_MusicMuted; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
	bool IsEffectsMuted() const { return m_EffectsMuted; }

private:
	const float DEFAULTVALUE = 50.f;
	float m_MasterVolume = DEFAULTVALUE;
	float m_MusicVolume = DEFAULTVALUE;
	float m_EffectsVolume = DEFAULTVALUE;

	bool m_MasterMuted = false;
	bool m_MusicMuted = false;
	bool m_EffectsMuted = false;
};
