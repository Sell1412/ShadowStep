// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "YAToolsSettings.generated.h"

class UEditorUtilityWidgetBlueprint;

UCLASS(Config=Editor, defaultconfig, meta=(DisplayName="YA Tools Settings"))
class YATOOLS_API UYAToolsSettings : public UDeveloperSettings {
	GENERATED_BODY()
	
public:
    UPROPERTY(EditAnywhere, Config, Category = "Audio|Sound Stack Editor")
    TSoftObjectPtr<UEditorUtilityWidgetBlueprint> SoundStackEditorClass{ nullptr };
};
