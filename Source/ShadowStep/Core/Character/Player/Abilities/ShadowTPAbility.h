// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BaseAbility.h"
#include "ShadowTPAbility.generated.h"

class UShadowManager;

/**
 * 
 */
UCLASS(Blueprintable)
class SHADOWSTEP_API UShadowTPAbility : public UBaseAbility
{
	GENERATED_BODY()
	
protected:

	UFUNCTION(BlueprintCallable, Category = "Ability")
	void Telport();

	UFUNCTION(BlueprintCallable, Category = "Ability")
	bool TryValidateTeleportLocation();

	UPROPERTY(BlueprintReadWrite, Category = "Teleportation")
	bool isLocationValid = false;

	UPROPERTY(BlueprintReadWrite, Category = "Teleportation")
	FVector TPLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, Category = "Teleportation")
	FVector TPNormal = FVector::ZeroVector;
	
	UPROPERTY(BlueprintReadWrite, Category = "Teleportation")
	bool isLastTPPointValid = false;

	UPROPERTY(BlueprintReadWrite, Category = "Teleportation")
	FVector lastValidTPPoint = FVector::ZeroVector;
	

	UPROPERTY(BlueprintReadWrite, Category = "Teleportation")
	FVector modifyedTPLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, Category = "Teleportation")
	FVector lastValidTPNormal = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, Category = "Teleportation")
	FVector lastModifyedTPLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, Category = "Teleportation")
	float validTeleportPointRadius = 100.f;
};
