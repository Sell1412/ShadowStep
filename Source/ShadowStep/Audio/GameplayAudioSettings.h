// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GameplayAudioSettings.generated.h"

UCLASS(Config=Game, defaultconfig, meta = (DisplayName = "Gameplay Audio Settings"))
class SHADOWSTEP_API UGameplayAudioSettings : public UDeveloperSettings {
	GENERATED_BODY()
	
public:
	UPROPERTY(Config, EditAnywhere, Category = "Loading")
	TSoftObjectPtr<class ULevelStackMap> LevelStackMap{ nullptr };

	UPROPERTY(Config, EditAnywhere, Category = "Volume Management")
	TSoftObjectPtr<class USoundControlBusMix> DesignerControlBusMix{};

	UPROPERTY(Config, EditAnywhere, Category = "Volume Management")
	TMap<TSoftObjectPtr<class USoundControlBus>, float> DefaultBusVolumes{};

	UPROPERTY(Config, EditAnywhere, Category = "Music")
	TSoftObjectPtr<class ULevelMusicMap> LevelMusicMap{ nullptr };

	UPROPERTY(Config, EditAnywhere, Category = "Pooling")
	int StartPoolItems{ 15 };

	// The maximum amount of sounds to be simultaneously played at a given time.
	UPROPERTY(Config, EditAnywhere, Category = "Pooling")
	int MaximumPoolItems{ 50 };
};
