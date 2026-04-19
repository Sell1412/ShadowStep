// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.

#pragma once


#include "AIController.h"
#include "Perception/AISenseConfig.h"
#include "Perception/AISense.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionComponent.h"

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AISenseLibary.generated.h"


/**
 * 
 */
UCLASS()
class SHADOWSTEP_API UAISenseLibary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable)
		static UAISenseConfig* GetPerceptionSenseConfig(AAIController* Controller, TSubclassOf<UAISense> SenseClass);

	/// <summary>
	/// Sets the new Radius for the Sight AI Component
	/// </summary>
	/// <param name="Controller"></param>
	/// <param name="SightRange"></param>
	/// <returns></returns>
	UFUNCTION(BlueprintCallable)
		static bool SetSightRange(AAIController* Controller, float SightRange);

	/// <summary>
	/// Sets the Width in Angle for the Detection Cone
	/// </summary>
	/// <param name="Controller"></param>
	/// <param name="SightAngle"></param>
	/// <returns></returns>
	UFUNCTION(BlueprintCallable)
		static bool SetSightAngle(AAIController* Controller, float SightAngle);
};
