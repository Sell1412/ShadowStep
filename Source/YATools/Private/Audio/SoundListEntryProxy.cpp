// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.

#include "Audio/SoundListEntryProxy.h"

#include "ShadowStep.h"
#include "Audio/SoundStack.h"
#include "Audio/SoundStackEditor.h"
#include "Audio/SoundListEntryRowWidget.h"

#if WITH_EDITOR
void USoundListEntryProxy::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) {
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (!OwnerStack) return;

	FName PropertyName = PropertyChangedEvent.GetPropertyName();

	// Check if the modified property was a tag -> update list view
	if (PropertyName == GET_MEMBER_NAME_CHECKED(FSoundDefinition, SoundTag)) {
		auto& NewTag = EditableEntry.SoundTag;
		if (!NewTag.IsValid()) {
			OnTagModified.Broadcast(true, NewTag);
			NewTag = OriginalTag; return;
		}

		if (NewTag == OriginalTag) {
			OnTagModified.Broadcast(false, NewTag);
			return; // Do nothing
		}

		//  Check if the tag already exists
		if (OwnerStack->SoundDatabase.Contains(NewTag)) {
			// Broadcast fail (before restoring again)
			OnTagModified.Broadcast(true, NewTag);

			// Restore original
			NewTag = OriginalTag;
			return;
		}

		// New tag (not used already) selected
		OwnerStack->Modify();

		const int32 DisplayOrderIndex = OwnerStack->SoundDisplayOrder.Find(OriginalTag);
		if (DisplayOrderIndex != INDEX_NONE) {
			OwnerStack->SoundDisplayOrder[DisplayOrderIndex] = NewTag;
		}

		OwnerStack->SoundDatabase.Remove(OriginalTag);
		OwnerStack->SoundDatabase.Add(NewTag, EditableEntry);
		OriginalTag = NewTag;
		OwnerStack->MarkPackageDirty();

		// Update Row UI
		if (USoundStackEditor* SoundStackEditor = Cast<USoundStackEditor>(GetOuter())) {
			if (AssignedRow)
				AssignedRow->UpdateDisplayedName(this);
		}

		OnTagModified.Broadcast(false, NewTag);
	}
	// Changed something other than a tag -> Update original data
	else if (OwnerStack->SoundDatabase.Contains(OriginalTag)) {
		OwnerStack->SoundDatabase[OriginalTag] = EditableEntry;
		OwnerStack->MarkPackageDirty();
	}
}
#endif