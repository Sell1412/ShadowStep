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
	void  Tick();
	virtual void Tick_Implementation();


	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Ability")
	void  OnAbilityActivate();
	virtual void OnAbilityActivate_Implementation();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Ability")
	void  OnAbilityDeactivated();
	virtual void OnAbilityDeactivated_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Ability")
	void Initialise(UShadowManager* a_ShadowManger);
	virtual void Initialise_Implementation(UShadowManager* a_ShadowManger);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Ability")
	void InteractionButtonStarted();
	virtual void InteractionButtonStarted_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Ability")
	void InteractionButtonReleased();
	virtual void InteractionButtonReleased_Implementation();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability", meta = (AllowPrivateAccess=true))
	float ManaCost = 0;
};
