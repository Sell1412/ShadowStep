// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BaseAbility.h"
#include "ShadowTPAbility.generated.h"

class UShadowManager;
class UCameraComponent;

/**
 * 
 */
UCLASS(Blueprintable)
class SHADOWSTEP_API UShadowTPAbility : public UBaseAbility
{
	GENERATED_BODY()
	
protected:

	// Functions
	//-------------------------------------------------------
	UFUNCTION(BlueprintCallable, Category = "Ability")
	bool TryValidateTeleportLocation();

	UFUNCTION(BlueprintCallable, Category = "Ability")
	bool GetHitPointOverTheEdge(FHitResult& a_outHit);


	// Properties
	//-------------------------------------------------------

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

	UPROPERTY(BlueprintReadWrite, Category = "Player")
	float PlayerRadius = 20.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Player")
	float PlayerHalfHeight = 90.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Offset")
	float HitPointExtendHightOffset = 250.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Offset")
	float HitPointExtendDownOffset = -400.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Wall")
	float CameraHitPointExtend = 50.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Components")
	TObjectPtr<UShadowManager> ShadowManager = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Components")
	TObjectPtr<UCameraComponent> PlayerCamera = nullptr;
};
