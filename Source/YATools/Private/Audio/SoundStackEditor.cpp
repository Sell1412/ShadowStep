// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.

#include "Audio/SoundStackEditor.h"

#include "Factories/DataAssetFactory.h"
#include "UObject/SavePackage.h"
#include "Audio/SoundStack.h"
#include "AssetToolsModule.h"
#include "ShadowStep.h"
#include "Audio/SoundListEntryProxy.h"
#include "EditorUtilityWidgetComponents.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Components/DetailsView.h"

void USoundStackEditor::NativeConstruct() {
	Super::NativeConstruct();

	LastSelectedStack = nullptr;

	// If we open the widget, we instantly populate the stack list
	PopulateStackList();

	// Bind events
	if (IsValid(CreateNewStackButton)) {
		CreateNewStackButton->OnClicked.AddDynamic(this, &USoundStackEditor::HandleCreateNewStackClicked);
	}

	if (IsValid(CreateNewSoundTagEntryButton)) {
		CreateNewSoundTagEntryButton->OnClicked.AddDynamic(this, &USoundStackEditor::HandleCreateNewSoundTagEntryClicked);
	}

	if (IsValid(StackEntriesListView)) {
		StackEntriesListView->OnItemSelectionChanged().AddUObject(this, &USoundStackEditor::HandleStackItemSelectionChanged);
	}

	if (IsValid(SoundTagEntriesListView)) {
		SoundTagEntriesListView->OnItemSelectionChanged().AddUObject(this, &USoundStackEditor::HandleSoundTagItemSelectionChanged);
	}

	if (IsValid(NewStackNameTextBox)) {
		NewStackNameTextBox->OnTextChanged.AddDynamic(this, &USoundStackEditor::HandleNewStackNameTextChanged);
	}

	if (IsValid(SoundTagSearchTextBox)) {
		SoundTagSearchTextBox->OnTextChanged.AddDynamic(this, &USoundStackEditor::HandleSoundTagSearchTextChanged);
	}

	PopulateSoundTagList(LastSelectedStack);
}

void USoundStackEditor::CreateNewStack(const FString& NewStackName, const FString& SavePath) {
	if (NewStackName.IsEmpty() || SavePath.IsEmpty()) {
		ULOGERROR("Cannot create Sound Stack! Name Or Path is empty!"); return;
	}

	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	UDataAssetFactory* Factory = NewObject<UDataAssetFactory>();
	Factory->DataAssetClass = USoundStack::StaticClass();

	UObject* NewAsset = AssetToolsModule.Get().CreateAsset(NewStackName, SavePath, USoundStack::StaticClass(), Factory);
	if (USoundStack* NewLibrary = Cast<USoundStack>(NewAsset)) {
		UPackage* Package = NewLibrary->GetOutermost();
		if (Package) {
			FString PackageFileName = FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension());
			FSavePackageArgs SaveArgs;
			SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
			SaveArgs.Error = GError;
			UPackage::SavePackage(Package, NewLibrary, *PackageFileName, SaveArgs);
		}
	}
}

void USoundStackEditor::HandleTagModified(bool bFailed, FGameplayTag AttemptedTag) {
	if (bFailed) return;


}

void USoundStackEditor::PopulateStackList() {
	if (!IsValid(StackEntriesListView)) return;

	StackEntriesListView->ClearListItems();

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> AssetData{};

	AssetRegistryModule.Get().GetAssetsByClass(FTopLevelAssetPath(USoundStack::StaticClass()->GetPathName()), AssetData);
	for (FAssetData& Asset : AssetData) {
		if (USoundStack* FoundStackAsset = Cast<USoundStack>(Asset.GetAsset())) {
			StackEntriesListView->AddItem(FoundStackAsset);
		}
	}

	// Select the first stack if no stack is already selected
	if (!LastSelectedStack && SoundTagEntriesListView->GetNumItems() > 0) LastSelectedStack = Cast<USoundStack>(SoundTagEntriesListView->GetItemAt(0));
}

void USoundStackEditor::PopulateSoundTagList(USoundStack* ForStack, bool bSelectLastEntry /*= false*/) {
	if (!ForStack || !IsValid(SoundTagEntriesListView)) return;

	// Also take search text into account
	const FString SearchText = CurrentSearchText.ToString().TrimStartAndEnd();

	SoundTagEntriesListView->ClearSelection();
	SoundTagEntriesListView->ClearListItems();

	ActiveSoundTagEntryProxies.Reset();

	// Reset to default, so no entry is highlighted before highlighting/selecting the last entry
	if (bSelectLastEntry) {
		LastSelectedCombo = FStackTagSelectionCombo{};
	}

	// Start from the back so newly added sounds are displayed at the top
	for (int32 SoundDisplayOrderIndex = ForStack->SoundDisplayOrder.Num() - 1; SoundDisplayOrderIndex >= 0; --SoundDisplayOrderIndex) {
		const FGameplayTag& SoundTag = ForStack->SoundDisplayOrder[SoundDisplayOrderIndex];
		if (!SoundTagContainsText(SoundTag, SearchText)) continue;

		FSoundDefinition* SoundDefinition = ForStack->SoundDatabase.Find(SoundTag);
		if (!SoundDefinition) continue;

		USoundListEntryProxy* CurrentProxy = NewObject<USoundListEntryProxy>(this);
		if (!IsValid(CurrentProxy)) continue;

		ActiveSoundTagEntryProxies.Add(CurrentProxy);

		CurrentProxy->OriginalTag = ForStack->SoundDisplayOrder[SoundDisplayOrderIndex];
		CurrentProxy->OwnerStack = ForStack;
		CurrentProxy->OwnerEditor = this;
		CurrentProxy->EditableEntry = *ForStack->SoundDatabase.Find(ForStack->SoundDisplayOrder[SoundDisplayOrderIndex]);

		CurrentProxy->OnTagModified.AddDynamic(this, &USoundStackEditor::HandleTagModified);
		SoundTagEntriesListView->AddItem(CurrentProxy);

		// Check if last item should be selected
		if (bSelectLastEntry && SoundDisplayOrderIndex == ForStack->SoundDisplayOrder.Num() - 1) {
			// Select entry
			SoundTagEntriesListView->SetSelectedItem(CurrentProxy);

			// Update combo
			LastSelectedCombo.SoundStack = ForStack;
			LastSelectedCombo.SoundTag = ForStack->SoundDisplayOrder[SoundDisplayOrderIndex];
		}
	}

	SoundTagEntriesListView->RegenerateAllEntries();
}

void USoundStackEditor::AddNewSoundTagEntry() {
	if (!LastSelectedStack) return;

	const FGameplayTag NewDefaultKey = FGameplayTag::RequestGameplayTag(TEXT("Audio.Sounds"), false);
	if (!NewDefaultKey.IsValid()) return; // Tag does not exist

	if (LastSelectedStack->SoundDatabase.Contains(NewDefaultKey)) return; // Stack already contains default tag

	LastSelectedStack->Modify();
	
	FSoundDefinition NewDefinition{};
	NewDefinition.SoundTag = NewDefaultKey;

	LastSelectedStack->SoundDatabase.Add(NewDefaultKey, NewDefinition);
	LastSelectedStack->SoundDisplayOrder.Add(NewDefaultKey);

	LastSelectedStack->MarkPackageDirty();

	// Update the sound tag list after adding a new sound tag entry and select the new sound tag entry
	PopulateSoundTagList(LastSelectedStack, true);
}

void USoundStackEditor::HandleCreateNewStackClicked() {
	if (!IsValid(NewStackNameTextBox) || !IsValid(NewStackSavePathTextBox)) return;

	CreateNewStack(NewStackNameTextBox->GetText().ToString(), NewStackSavePathTextBox->GetText().ToString());
}

void USoundStackEditor::HandleCreateNewSoundTagEntryClicked() {
	AddNewSoundTagEntry();
}

void USoundStackEditor::HandleNewStackNameTextChanged(const FText& Text) {
	if (!IsValid(NewStackNameTextBox)) return;
	
	const FString& TextString = Text.ToString();
	if(TextString.Len() > MaximumSoundStackNameLength) {
		// Limit text length
		NewStackNameTextBox->SetText(FText::FromString(TextString.Mid(0, MaximumSoundStackNameLength)));
	}
}

void USoundStackEditor::HandleSoundTagSearchTextChanged(const FText& Text) {
	if (!LastSelectedStack) return;
	CurrentSearchText = Text;

	// Repopulate the sound tag list based on the search text
	PopulateSoundTagList(LastSelectedStack);



	// Only show stacks with tags containing the text

	//if (!IsValid(StackEntriesListView)) return;

	//TArray<UObject*> ContainingStacks{};

	//for (UObject* StackEntriesObject : StackEntriesListView->GetListItems()) {
	//	if (!StackEntriesObject) continue;
	//	USoundStack* SoundStackEntry = Cast<USoundStack>(StackEntriesObject);
	//	if (!SoundStackEntry) continue;

	//	// Check if the stack contains a a tag of the type
	//	for (auto const& [Tag, _] : SoundStackEntry->SoundDatabase) {
	//		if (Tag.GetTagName().ToString().Contains(Text.ToString())) {
	//			ContainingStacks.Add(SoundStackEntry);
	//		}
	//	}
	//}

	//StackEntriesListView->ClearListItems();
	//StackEntriesListView->ClearSelection();

	//// Only display stacks containing tags with the search text
	//StackEntriesListView->SetListItems(ContainingStacks);
}

void USoundStackEditor::HandleStackItemSelectionChanged(UObject* Item) {
	if (!IsValid(StackEntriesListView)) return;

	USoundStack* NewSelectedStack = Cast<USoundStack>(Item);
	if (!NewSelectedStack) return;

	if (NewSelectedStack == LastSelectedStack) return;

	// Cache new stack reference
	LastSelectedStack = NewSelectedStack;

	// Update sound tag list view
	PopulateSoundTagList(NewSelectedStack);
}

void USoundStackEditor::HandleSoundTagItemSelectionChanged(UObject* Item) {
	if (!IsValid(SoundTagEntryDisplay) || !LastSelectedStack) return;

	USoundListEntryProxy* EntryProxy = Cast<USoundListEntryProxy>(Item);
	if (!EntryProxy) return;

	FSoundDefinition* SoundDefinition = LastSelectedStack->SoundDatabase.Find(EntryProxy->OriginalTag);
	if (!SoundDefinition) return;

	// Update proxy values
	EntryProxy->EditableEntry.SoundTag = SoundDefinition->SoundTag;
	EntryProxy->EditableEntry.SoundFile = SoundDefinition->SoundFile;
	EntryProxy->EditableEntry.SurfaceSoundMap = SoundDefinition->SurfaceSoundMap;
	EntryProxy->EditableEntry.ConcurrencyOverride = SoundDefinition->ConcurrencyOverride;
	EntryProxy->EditableEntry.AttenuationOverride = SoundDefinition->AttenuationOverride;

	// Display the proxy
	SoundTagEntryDisplay->SetObject(EntryProxy);

	// Update selected combo
	LastSelectedCombo.SoundTag = SoundDefinition->SoundTag;
	LastSelectedCombo.SoundStack = LastSelectedStack;
}

bool USoundStackEditor::SoundTagContainsText(const FGameplayTag& SoundTag, const FString& SearchText) {
	if (SearchText.IsEmpty()) return true;
	if (!SoundTag.IsValid()) return false;

	FString TagString = SoundTag.GetTagName().ToString();
	TagString.RemoveFromStart(TEXT("Audio.Sounds."));

	return TagString.Contains(SearchText, ESearchCase::IgnoreCase);
}