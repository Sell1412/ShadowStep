/*****************************************************************************
* Project: OutOfBounce
* File : LoadingHelper.h
* Author : Julian Serve
*
* CopyRight (c) 2026 Core Memory Entertainment. All Rights Reserved.
*
* Description:
* 'ULoadingHelper' is a utility class that provides a static function to transition to
* a new level using the GameLoadingSubsystem.
*
******************************************************************************/
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LoadingHelper.generated.h"

UCLASS()
class SHADOWSTEP_API ULoadingHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	/// <summary>
	/// Transitions to a new level using the GameLoadingSubsystem, which handles the loading screen and asynchronous level loading.
	/// </summary>
	/// <param name="WorldContextObject"></param>
	/// <param name="LevelToLoad"></param>
	/// <param name="UseLoadingScreen"></param>
	/// <param name="MinLoadingTime"></param>
	UFUNCTION(BlueprintCallable, Category = "Own | Loading", meta = (WorldContext = "WorldContextObject"))
	static void TransitionToLevel(const UObject* WorldContextObject, TSoftObjectPtr<UWorld> LevelToLoad, bool UseLoadingScreen = true, float MinLoadingTime = 2.0f);
};
