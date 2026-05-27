// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.

#include "ShadowStoneAbility.h"
#include "Camera/PlayerCameraManager.h"

void UShadowStoneAbility::CalculateVelocity(FVector StartPoint, FVector& LaunchVelocity)
{
	if(!ShadowManager)
	{
		UE_LOGFMT(LogTemp, Warning, "Shadow Manager is not set in ShadowStoneAbility");
		LaunchVelocity = FVector::ZeroVector;
		return;
	}

	if(!PlayerCamera)
	{
		UE_LOGFMT(LogTemp, Warning, "Player Camera is not set in ShadowStoneAbility");
		LaunchVelocity = FVector::ZeroVector;
		return;
	}

	FVector cameraLocation = PlayerCamera->GetCameraLocation();
	FVector cameraForwardVector = PlayerCamera->GetCameraRotation().Vector();

	FVector lineTraceEndPoint = cameraForwardVector * MaxThrowDistance + cameraLocation;

	FHitResult hitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(ShadowManager->GetOwner());
	GetWorld()->LineTraceSingleByChannel(hitResult, cameraLocation, lineTraceEndPoint, ECollisionChannel::ECC_Visibility, Params);

	FVector hitLocation = hitResult.bBlockingHit ? hitResult.Location : lineTraceEndPoint;


	float calculatedFlightTime = FVector::Distance(hitLocation, StartPoint)/NormalThrowSpeed;
	FVector baseVelocity = (hitLocation - StartPoint) / calculatedFlightTime;
	float gravityZ = (ShadowManager->GetWorld()->GetGravityZ() * 0.5)* calculatedFlightTime;
	FVector gravityOffsetVector = FVector(0.0,0.0,gravityZ);

	LaunchVelocity = baseVelocity - gravityOffsetVector;
}
