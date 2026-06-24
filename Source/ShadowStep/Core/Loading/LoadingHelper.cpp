//  CopyRight (c) 2026 Core Memory Entertainment. All Rights Reserved.

#include "LoadingHelper.h"
#include "Kismet/GameplayStatics.h"
#include "GameLoadingSubsystem.h"


void ULoadingHelper::TransitionToLevel(const UObject* WorldContextObject, TSoftObjectPtr<UWorld> LevelToLoad, bool UseLoadingScreen /*= true*/, float MinLoadingTime /*= 2.0f*/) {

	// Get game instance
	UGameInstance* gameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
	if (!gameInstance) {
		UE_LOGFMT(LogTemp, Warning, "'LoadingHelper' could not find the 'GameInstance'!"); return;
	}

	// Get subsystem
	UGameLoadingSubsystem* loadingSubsystem = gameInstance->GetSubsystem<UGameLoadingSubsystem>();
	if (!loadingSubsystem) {
		UE_LOGFMT(LogTemp, Warning, "'LoadingHelper' could not find the 'GameLoadingSubsystem'!"); return;
	}

	// Call transition
	loadingSubsystem->TransitionToLevel(LevelToLoad, UseLoadingScreen, MinLoadingTime);
}
