// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.


#include "Audio/SoundStack.h"

TArray<FSoftObjectPath> USoundStack::GetAssetsToLoad() const {
	TArray<FSoftObjectPath> AssetsToLoad{};

	for (const auto& Pair : SoundDatabase) {
		const FSoundDefinition& SoundDefinition = Pair.Value;

		// Check the surface map for sounds
		if (SoundDefinition.SurfaceSoundMap.Num() > 0) {
			for (const auto& SurfaceSoundPair : SoundDefinition.SurfaceSoundMap) {
				if (SurfaceSoundPair.Value.IsNull()) continue;
				const FSoftObjectPath& SoundPath = SurfaceSoundPair.Value.ToSoftObjectPath();
				if (SoundPath.IsValid()) AssetsToLoad.Add(SoundPath);
			}
		}
		else if(!SoundDefinition.SoundFile.IsNull()) { // Use the default sound
			const FSoftObjectPath& SoundPath = SoundDefinition.SoundFile.ToSoftObjectPath();
			if (SoundPath.IsValid()) AssetsToLoad.Add(SoundPath);
		}
	}

	return AssetsToLoad;
}
