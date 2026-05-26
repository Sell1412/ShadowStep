// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.

#include "ShadowTPAbility.h"

void UShadowTPAbility::Telport()
{
	UE_LOGFMT(LogTemp, Warning, "Teleport");
}

bool UShadowTPAbility::TryValidateTeleportLocation()
{
	// If the last teleport point is valid, check if the current teleport point is within a certain radius of the last valid teleport point
	if (isLastTPPointValid)
	{
		// If the teleport point is in the range of last valid tppoint, set this as current teleport point
		float distance = FVector::Distance(TPLocation, lastValidTPPoint);
		if (distance <= validTeleportPointRadius)
		{
			TPLocation = lastValidTPPoint;
			TPNormal = lastValidTPNormal;
			modifyedTPLocation = lastModifyedTPLocation;
			return true;
		}
		// Set last valid tppoint invalid
		else
		{
			isLastTPPointValid = false;
			lastValidTPNormal = FVector::ZeroVector;
			lastValidTPPoint = FVector::ZeroVector;
			lastModifyedTPLocation = FVector::ZeroVector;
			return false;
		}
	}

	isLocationValid = false;
	TPLocation = FVector::ZeroVector;
	TPNormal = FVector::ZeroVector;
	return false;
}
