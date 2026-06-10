// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "../Manager/ShadowManager.h"
#include "../../../Enums/ShadowAbilityType.h"
#include "BaseAbility.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilityDeactivated, float, CoolDownInSeconds);

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

	UPROPERTY(BlueprintCallable, EditAnywhere, BlueprintAssignable)
	FOnAbilityDeactivated OnAbilitySuccessfullyUsed;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability", meta = (AllowPrivateAccess=true))
	float manaCost = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	float cooldown = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	bool canUseAbility = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	EShadowAbilityType shadowAbilityType = EShadowAbilityType::SAT_TP;
};
