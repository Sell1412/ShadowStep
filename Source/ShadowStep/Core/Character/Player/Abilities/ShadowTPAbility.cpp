// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.

#include "ShadowTPAbility.h"
#include "Camera/CameraComponent.h"


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
		}
	}

	isLocationValid = false;
	TPLocation = FVector::ZeroVector;
	TPNormal = FVector::ZeroVector;
	return false;
}

bool UShadowTPAbility::GetHitPointOverTheEdge(FHitResult& a_outHit)
{
	if (!PlayerCamera)
	{
		UE_LOGFMT(LogTemp, Warning, "Player Camera is not set in ShadowTPAbility");
		return false;
	}

	// Calculate the start and end location for the sweep test based on the player's camera forward vector, the teleport location, and the defined offsets
	FVector forwardVector = PlayerCamera->GetForwardVector()*CameraHitPointExtend;
	FVector startLocation = forwardVector + TPLocation + FVector(0.0f, 0.0f, HitPointExtendHightOffset);
	FVector endLocation = startLocation + FVector(0.0f, 0.0f, HitPointExtendDownOffset);

	// Perform a sweep test using a sphere collision shape to check for valid teleport locations
	FCollisionShape collisionShape = FCollisionShape::MakeSphere(PlayerRadius);
	bool outHit = GetWorld()->SweepSingleByChannel (a_outHit, startLocation, endLocation, FQuat::Identity, ECC_Visibility, collisionShape);

	return outHit;
}
