// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"

#include "Checkpoint.generated.h"

UCLASS()
class SHADOWSTEP_API ACheckpoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACheckpoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;



public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Default")
	bool bShouldLoadNewArea{false};

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Default", meta = (EditCondition = "bShouldLoadNewArea", EditConditionHides))
	FGameplayTag OldArea;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Default",meta = (EditCondition = "bShouldLoadNewArea", EditConditionHides))
	FGameplayTag NewArea;

};
