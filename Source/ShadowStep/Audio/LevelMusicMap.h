// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.

#pragma once

#include "GameplayTagContainer.h"

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LevelMusicMap.generated.h"

USTRUCT(BlueprintType)
struct FMusicTransition {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag MusicTag{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MusicFadeDuration{ 1.0f };
};

UCLASS()
class SHADOWSTEP_API ULevelMusicMap : public UPrimaryDataAsset {
	GENERATED_BODY()
	
public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override {
		return FPrimaryAssetId("LevelMusicMap", GetFName());
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<TSoftObjectPtr<UWorld>, FMusicTransition> LevelMusicMap{};
};
