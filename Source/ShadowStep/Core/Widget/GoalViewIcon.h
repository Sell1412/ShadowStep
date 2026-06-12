// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GoalViewIcon.generated.h"

class APlayerController;
/**
 * 
 */
UCLASS()
class SHADOWSTEP_API UGoalViewIcon : public UUserWidget
{
	GENERATED_BODY()
	

	public:
	UFUNCTION(BlueprintCallable)
	void UpdateScreenPos(APlayerController* PlayerController, FVector GoalPosition);
};
