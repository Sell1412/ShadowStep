// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.

#include "ShadowTPAbility.h"
#include "Logging/StructuredLog.h"

void UShadowTPAbility::Telport()
{
	UE_LOGFMT(LogTemp, Warning, "Teleport");
}

void UShadowTPAbility::SetLocationNotValid()
{
	if(isLocationValid)
	{
		isLocationValid = false;
		TPLocation = FVector::ZeroVector;
		TPNormal = FVector::ZeroVector;
	}
}
