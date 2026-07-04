// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.

#pragma once

#include "GameplayTagContainer.h"

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SoundStack.generated.h"

USTRUCT(BlueprintType)
struct FSoundDefinition {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag SoundTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	TSoftObjectPtr<USoundBase> SoundFile{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	TMap<TEnumAsByte<EPhysicalSurface>, TSoftObjectPtr<USoundBase>> SurfaceSoundMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USoundConcurrency> ConcurrencyOverride{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USoundAttenuation> AttenuationOverride{ nullptr };

	USoundBase* GetSoundForSurface(EPhysicalSurface SurfaceType = SurfaceType_Default) const {
		if (SurfaceSoundMap.Contains(SurfaceType)) {
			return SurfaceSoundMap[SurfaceType].LoadSynchronous(); // Should be loaded already at this point
		}

		return nullptr;
	}
};

UCLASS()
class SHADOWSTEP_API USoundStack : public UPrimaryDataAsset {
	GENERATED_BODY()
	
public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override {
		return FPrimaryAssetId("SoundStack", GetFName());
	}

	TArray<FSoftObjectPath> GetAssetsToLoad() const;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FGameplayTag, FSoundDefinition> SoundDatabase;

	UPROPERTY(EditAnywhere)
	TArray<FGameplayTag> SoundDisplayOrder{};
};
