/*****************************************************************************
* Project: OutOfBounce
* File : GameLoadingSettings.h
* Author : Julian Serve
*
* CopyRight (c) 2026 Core Memory Entertainment. All Rights Reserved.
*
* Description:
* 'UGameLoadingSettings' is a configuration class that holds settings related to the game's loading process.
* It includes references to the loading screen widget class and a transition map to be used during level transitions.
******************************************************************************/
#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GameLoadingSettings.generated.h"

UCLASS(Config=Game, defaultconfig, meta = (DisplayName = "Game Loading Settings"))
class SHADOWSTEP_API UGameLoadingSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Config, EditAnywhere, meta = (DisplayName = "Loading Screen Widget Class"), Category = "UI")
	TSubclassOf<UUserWidget> LoadingScreenWidgetClass{ nullptr };

	UPROPERTY(Config, EditAnywhere, meta = (DisplayName = "Transition Map"), Category = "Maps")
	TSoftObjectPtr<UWorld> TransitionMap{ nullptr };
};
