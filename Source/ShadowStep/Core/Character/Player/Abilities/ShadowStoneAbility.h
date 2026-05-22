// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BaseAbility.h"
#include "ShadowStoneAbility.generated.h"

class UShadowManager;

/**
 * 
 */
UCLASS(Blueprintable)
class SHADOWSTEP_API UShadowStoneAbility : public UBaseAbility
{
	GENERATED_BODY()

	virtual void Tick_Implementation() override;

	virtual void OnAbilityActivate_Implementation() override;

	virtual	void OnAbilityDeactivated_Implementation() override;

	virtual void Initialise_Implementation(UShadowManager* a_ShadowManger) override;

	virtual void InteractionButtonStarted_Implementation() override;

	virtual void InteractionButtonReleased_Implementation() override;
};
