// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.


#include "AISenseLibary.h"

UAISenseConfig* UAISenseLibary::GetPerceptionSenseConfig(AAIController* Controller, TSubclassOf<UAISense> SenseClass)
{
	UAISenseConfig* result = nullptr;

	FAISenseID Id = UAISense::GetSenseID(SenseClass);
	if (!Id.IsValid())
	{
		// Given Sense Class invalid
		UE_LOG(LogTemp, Error, TEXT("GetPerceptionSenseConfig: Wrong Sense ID"));
	}
	else if (Controller == nullptr)
	{
		// Given Controller invalid
		UE_LOG(LogTemp, Error, TEXT("GetPerceptionSenseConfig: Controller == nullptr"));
	}
	else
	{
		// Get Perception Sense Config
		UAIPerceptionComponent* Perception = Controller->GetAIPerceptionComponent();
		if (Perception == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("GetPerceptionSenseConfig = No AI Component found"))
		}
		else
		{
			result = Perception->GetSenseConfig(Id);
		}
	}
	return result;

}

bool UAISenseLibary::SetSightRange(AAIController* Controller, float SightRange)
{
	UAISenseConfig* config = GetPerceptionSenseConfig(Controller, UAISense_Sight::StaticClass());

	if (config == nullptr) 
	{
		UE_LOG(LogTemp, Error, TEXT("SetSightRange = no Sight sense Found in given AI Controller"))
		return false;
	}
	UAISenseConfig_Sight* configSight = Cast<UAISenseConfig_Sight>(config);

	// Save original Lose sight difference
	float loseRange = configSight->LoseSightRadius - configSight->SightRadius;

	configSight->SightRadius = SightRange;

	configSight->LoseSightRadius = SightRange + loseRange;

	UAIPerceptionComponent* Perception = Controller->GetAIPerceptionComponent();

	// Update to new Values
	Perception->RequestStimuliListenerUpdate();

	return true;
}

bool UAISenseLibary::SetSightAngle(AAIController* Controller, float SightAngle)
{
	UAISenseConfig* config = GetPerceptionSenseConfig(Controller, UAISense_Sight::StaticClass());

	if (config == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("SetSightRange = no Sight sense Found in given AI Controller"))
			return false;
	}
	UAISenseConfig_Sight* configSight = Cast<UAISenseConfig_Sight>(config);

	configSight->PeripheralVisionAngleDegrees = SightAngle;

	UAIPerceptionComponent* Perception = Controller->GetAIPerceptionComponent();

	// Update to new Values
	Perception->RequestStimuliListenerUpdate();

	return true;
}
