// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SuspicionIndicator.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class SHADOWSTEP_API USuspicionIndicator : public UUserWidget
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable,Category="Suspicion")
	float GetEnemyRotationAngle(FVector EnemyLocation) const;
	

	UFUNCTION(BlueprintCallable,Category="Suspicion")
	void SetWidgetOffestAndRotation(float OffsetDistance, float RotationToEnemyInScreenSpace,FVector EnemyWorldPosition);
	
	UFUNCTION(BlueprintCallable,Category="Suspicion")
	void UpdateOpacity(float SusValue, float DeltaTime);


	private:
	UPROPERTY(EditAnywhere, Category = "Suspicion")
	float currentOpacity = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Suspicion | Scale",meta=(AllowPrivateAccess=true))
	float maxWorldDistance = 1500.0f;

	UPROPERTY(EditAnywhere, Category = "Suspicion | Distance ",meta=(AllowPrivateAccess=true))
	float maxScreenDist = 300.0f;
};
