// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Abilities/BaseAbility.h"
#include "ShadowManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilityUsed, class UBaseAbility*, Ability);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilityNoMana, float, ManaCost);

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHADOWSTEP_API UShadowManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	UShadowManager();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	AActor* SpawnActor(UClass* a_objectToSpawn, FVector a_SpawnLocation);



public:
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnAbilityUsed OnAbilityUsed;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnAbilityNoMana OnAbilityNoMana;
protected:
	
	virtual void BeginPlay() override;

};
