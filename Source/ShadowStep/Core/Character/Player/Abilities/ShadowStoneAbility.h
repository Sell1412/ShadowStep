// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BaseAbility.h"
#include "ShadowStoneAbility.generated.h"

class APlayerCameraManager;

/**
 * 
 */
UCLASS(Blueprintable)
class SHADOWSTEP_API UShadowStoneAbility : public UBaseAbility
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintCallable)
	void CalculateVelocity(FVector StartPoint, FVector& LaunchVelocity);

	// Properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shadow Stone")
	float MaxThrowDistance = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shadow Stone")
	float NormalThrowSpeed = 500.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Components")
	TObjectPtr<UShadowManager> ShadowManager = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Components")
	TObjectPtr<APlayerCameraManager> PlayerCamera = nullptr;

	float m_calulatedFlightTime = 0.0f;

	FVector m_baseVelocity = FVector::ZeroVector;


};
