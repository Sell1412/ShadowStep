// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.

#include "ShadowTPAbility.h"
#include "Camera/CameraComponent.h"

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
		}
	}

	isLocationValid = false;
	TPLocation = FVector::ZeroVector;
	TPNormal = FVector::ZeroVector;
	return false;
}

bool UShadowTPAbility::GetHitPointOverTheEdge(FHitResult& a_outHit)
{
	if (!ew_playerCamera)
	{
		UE_LOGFMT(LogTemp, Warning, "Player Camera is not set in ShadowTPAbility");
		return false;
	}

	FVector forwardVector = ew_playerCamera->GetForwardVector()*ew_cameraHitPointExtend;
	FVector startLocation = forwardVector + TPLocation + FVector(0.0f, 0.0f, ew_HitPointExtendHightOffset);

	FVector endLocation = startLocation + FVector(0.0f, 0.0f, ew_hitPointExtendDownOffset);
	FCollisionShape collisionShape = FCollisionShape::MakeSphere(ew_playerRadius);
	bool outHit = GetWorld()->SweepSingleByChannel (a_outHit, startLocation, endLocation, FQuat::Identity, ECC_Visibility, collisionShape);

	return outHit;
}
