// Fill out your copyright notice in the Description page of Project Settings.

#include "Planet.h"


// Sets default values
APlanet::APlanet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Fill necessary data
	sizeSpeed      = 5.0f;
	minSize        = 1.0f;
	maxSize   	   = 20.0f;
	reductionState = true;
	growthState    = false;

	//Create sub components
	mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");

	//Set initial state
	SetActorRelativeScale3D(FVector(maxSize));

}

// Called when the game starts or when spawned
void APlanet::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlanet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Check if changing size
	if(reductionState || growthState)
	{
		//If reducing
		if(reductionState)
		{
			ReduceSize(DeltaTime);
		}

		//If growing
		if(growthState)
		{
			GrowSize(DeltaTime);
		}
	}
}

void APlanet::ReduceSize(float DeltaTime)
{
	//Scaling per time
	FVector scale = GetTransform().GetScale3D();
	scale -= FVector(DeltaTime * sizeSpeed);

	//Check if min size has been reached
	if(scale.X <= minSize)
	{
		scale = FVector(minSize);
	}
	SetActorRelativeScale3D(scale);
}

void APlanet::GrowSize(float DeltaTime)
{
	//Scaling per time
	FVector scale = GetTransform().GetScale3D();
	scale += FVector(DeltaTime * sizeSpeed);

	//Check if max size has been reached
	if(scale.X >= maxSize)
	{
		scale = FVector(maxSize);
	}
	SetActorRelativeScale3D(scale);
}