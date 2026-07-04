// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.

#pragma once

#include "Audio/SoundStack.h"

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SoundListEntryProxy.generated.h"

class USoundStack;
class USoundStackEditor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTagModified, bool, Failed, FGameplayTag, AttemptedTag);

UCLASS(BlueprintType)
class YATOOLS_API USoundListEntryProxy : public UObject {
	GENERATED_BODY()
	
protected:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:
	UPROPERTY(BlueprintAssignable)
	FOnTagModified OnTagModified;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data", meta = (ShowOnlyInnerProperties))
	FSoundDefinition EditableEntry;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<USoundStack> OwnerStack{ nullptr };

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<USoundStackEditor> OwnerEditor{ nullptr };

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag OriginalTag{};

	UPROPERTY(BlueprintReadWrite, Category = "Own|Sound List Entry Proxy")
	TObjectPtr<class USoundListEntryRowWidget> AssignedRow{ nullptr };
};
