// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.

#pragma once

#include "GameplayTagContainer.h"

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "SoundStackEditor.generated.h"

class UEditorUtilityEditableTextBox;
class UEditorUtilityListView;
class UEditorUtilityButton;
class USoundListEntryProxy;
class UDetailsView;
class USoundStack;

USTRUCT(BlueprintType)
struct FStackTagSelectionCombo {
	GENERATED_BODY()

	UPROPERTY(Transient, BlueprintReadOnly)
	TObjectPtr<USoundStack> SoundStack{ nullptr };

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag SoundTag{};
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFailedToAddEmptyTag);

UCLASS()
class YATOOLS_API USoundStackEditor : public UEditorUtilityWidget {
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	void CreateNewStack(const FString& NewStackName, const FString& SavePath);

	UFUNCTION()
	void HandleTagModified(bool bFailed, FGameplayTag AttemptedTag);

public:
	void PopulateStackList();

	/*
		Happens when selecting a new stack or adding a new sound tag entry
	*/
	void PopulateSoundTagList(USoundStack* ForStack, bool bSelectLastEntry = false);

	void AddNewSoundTagEntry();

private:
	UFUNCTION()
	void HandleCreateNewStackClicked();

	UFUNCTION()
	void HandleCreateNewSoundTagEntryClicked();

	UFUNCTION()
	void HandleNewStackNameTextChanged(const FText& Text);

	UFUNCTION()
	void HandleSoundTagSearchTextChanged(const FText& Text);

	// Doesn't need 'UFUNCTION' -> uses 'AddUObject'
	void HandleStackItemSelectionChanged(UObject* Item);

	// Doesn't need 'UFUNCTION' -> uses 'AddUObject'
	void HandleSoundTagItemSelectionChanged(UObject* Item);

	// Helpers
	bool SoundTagContainsText(const FGameplayTag& SoundTag, const FString& SearchText);

public:
	// UI
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UEditorUtilityButton> CreateNewStackButton{ nullptr };

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UEditorUtilityButton> CreateNewSoundTagEntryButton{ nullptr };

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr< UEditorUtilityEditableTextBox> SoundTagSearchTextBox{ nullptr };

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UEditorUtilityEditableTextBox> NewStackNameTextBox{ nullptr };

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UEditorUtilityEditableTextBox> NewStackSavePathTextBox{ nullptr };

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UEditorUtilityListView> StackEntriesListView{ nullptr };

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UEditorUtilityListView> SoundTagEntriesListView{ nullptr };

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UDetailsView> SoundTagEntryDisplay{ nullptr };

	UPROPERTY(BlueprintReadOnly, Transient)
	FStackTagSelectionCombo LastSelectedCombo{};

private:
	// Internal values
	UPROPERTY(Transient)
	TObjectPtr<USoundStack> LastSelectedStack{ nullptr };

	UPROPERTY(Transient)
	TArray<TObjectPtr<USoundListEntryProxy>> ActiveSoundTagEntryProxies{};

	UPROPERTY(EditAnywhere, Category = "YATools|Sound Stack Editor|Settings")
	int32 MaximumSoundStackNameLength{ 20 };

	FText CurrentSearchText{};
};
