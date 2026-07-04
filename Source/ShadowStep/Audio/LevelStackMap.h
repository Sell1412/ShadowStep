// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LevelStackMap.generated.h"

USTRUCT(BlueprintType)
struct FLevelStacks {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<TSoftObjectPtr<class USoundStack>> Stacks{};
};

UCLASS()
class SHADOWSTEP_API ULevelStackMap : public UPrimaryDataAsset {
	GENERATED_BODY()
	
public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override {
		return FPrimaryAssetId("LevelStackMap", GetFName());
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<TSoftObjectPtr<UWorld>, FLevelStacks> LevelStackMap{};
};
