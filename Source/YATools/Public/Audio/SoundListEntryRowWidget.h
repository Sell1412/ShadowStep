// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "SoundListEntryRowWidget.generated.h"

UCLASS()
class YATOOLS_API USoundListEntryRowWidget : public UEditorUtilityWidget {
	GENERATED_BODY()
	

public:
	UFUNCTION(BlueprintCallable, Category = "Own|Sound List Entry Row")
	void UpdateDisplayedName(class USoundListEntryProxy* Proxy);

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TagText{ nullptr };
};
