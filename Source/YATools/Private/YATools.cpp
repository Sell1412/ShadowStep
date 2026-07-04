#include "YATools.h"
#include "ShadowStep.h"
#include "YAToolsSettings.h"
#include "Audio/SoundStackEditor.h"
#include "PropertyEditorModule.h"
#include "Editor/Blutility/Public/EditorUtilitySubsystem.h"
#include "Editor/Blutility/Classes/EditorUtilityWidgetBlueprint.h"

void FYATools::StartupModule() {
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor")) {
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

		// Tell the editor to render our custom UI when encountering 'FComponentSelector'
		//PropertyModule.RegisterCustomPropertyTypeLayout(
		//	FComponentSelector::StaticStruct()->GetFName(),
		//	FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FComponentSelectorCustomization::MakeInstance)
		//);

		//PropertyModule.RegisterCustomPropertyTypeLayout(
		//	FDialogData::StaticStruct()->GetFName(),
		//	FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FDialogDataCustomization::MakeInstance)
		//);

		PropertyModule.NotifyCustomizationModuleChanged();

		UE_LOG(LogTemp, Warning, TEXT("YATools Module Loaded!"));
	}

#pragma region YATools Dropdown
	UToolMenu* MainMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu");
	UToolMenu* YAToolsMenu = MainMenu->AddSubMenu(
		"LevelEditor.MainMenu",
		NAME_None,
		"YATools",
		FText::FromString("YATools"),
		FText::FromString("Custom Project Tools")
	);

	// Add an audio section
	FToolMenuSection& AudioSection = YAToolsMenu->AddSection("Audio", FText::FromString("Audio"));

	AudioSection.AddMenuEntry(
		"SoundStacks",
		FText::FromString("Stacks"),
		FText::FromString("Manage All Sound Stacks"),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateRaw(this, &FYATools::OpenSoundStackWindow))
	);
#pragma endregion
}

void FYATools::ShutdownModule() {
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor")) {
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		//PropertyModule.UnregisterCustomPropertyTypeLayout(FComponentSelector::StaticStruct()->GetFName());
		//PropertyModule.UnregisterCustomPropertyTypeLayout(FDialogData::StaticStruct()->GetFName());
	}
}

IMPLEMENT_MODULE(FYATools, YATools)

void FYATools::OpenSoundStackWindow() {
	UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>();
	if (!EditorUtilitySubsystem) return;

	const UYAToolsSettings* Settings = GetDefault<UYAToolsSettings>();

	if (Settings && !Settings->SoundStackEditorClass.IsNull()) {
		UEditorUtilityWidgetBlueprint* WidgetBP = Settings->SoundStackEditorClass.LoadSynchronous();

		if (WidgetBP) {
			EditorUtilitySubsystem->SpawnAndRegisterTab(WidgetBP);
		}
	}
	else {
		ULOGERROR("Cannot open Sound Stack: No widget assigned in Project Settings -> YA Tools Settings.");
	}
}
