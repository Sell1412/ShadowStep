// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.


#include "ShadowManager.h"

// Sets default values for this component's properties
UShadowManager::UShadowManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UShadowManager::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UShadowManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

AActor* UShadowManager::SpawnActor(UClass* a_objectToSpawn, FVector a_SpawnLocation)
{
	if (!a_objectToSpawn)
	{
		return nullptr;
	}

	FTransform SpawnTransform(FRotator::ZeroRotator, a_SpawnLocation, FVector::OneVector);

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;


	AActor* spawnedActor = GetWorld()->SpawnActor(a_objectToSpawn, &SpawnTransform, SpawnParameters);

	return spawnedActor;
}