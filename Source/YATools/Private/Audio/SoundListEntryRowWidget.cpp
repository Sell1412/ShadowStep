
// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.

#include "Audio/SoundListEntryRowWidget.h"
#include "Audio/SoundListEntryProxy.h"
#include "Components/TextBlock.h"

void USoundListEntryRowWidget::UpdateDisplayedName(USoundListEntryProxy* Proxy) {
	if (!TagText || !Proxy) return;

	// Remove Audio.Sounds. from the start
	FString DisplayedName = Proxy->EditableEntry.SoundTag.GetTagName().ToString();
	DisplayedName.RemoveFromStart(TEXT("Audio.Sounds."));
	TagText->SetText(FText::FromString(DisplayedName));
}
