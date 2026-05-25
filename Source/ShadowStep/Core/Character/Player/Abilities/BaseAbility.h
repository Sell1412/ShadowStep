// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "../Manager/ShadowManager.h"
#include "BaseAbility.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class SHADOWSTEP_API UBaseAbility : public UObject
{
	GENERATED_BODY()
	
public:
	UBaseAbility(const FObjectInitializer& ObjectInizializer);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Ability")
	void  AbilityTick();
	virtual void  AbilityTick_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Ability")
	void ActivateAbility();
	virtual void ActivateAbility_Implementation();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Ability")
	void DeactivateAbility();
	virtual void DeactivateAbility_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Ability")
	void InitialiseAbility(UShadowManager* a_ShadowManger);
	virtual void InitialiseAbility_Implementation(UShadowManager* a_ShadowManger);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Ability")
	void StartInteraction();
	virtual void StartInteraction_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Ability")
	void ReleaseInteraction();
	virtual void ReleaseInteraction_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Ability")
	void ResetAbilityCooldown(float a_cooldown);
	virtual void ResetAbilityCooldown_Implementation(float a_cooldown);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability", meta = (AllowPrivateAccess=true))
	float manaCost = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	float cooldown = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	bool canUseAbility = false;
};
