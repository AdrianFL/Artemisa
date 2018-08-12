// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Planet.generated.h"

UCLASS()
class ARTEMISA_API APlanet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlanet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Reduce size of the planet
	void ReduceSize(float);

	//Augment the size of the planet
	void GrowSize(float);

	//============================================
	// VARIABLES
	//============================================

	//Growth variables
	UPROPERTY(EditAnywhere) 	//Reduction speed
	float sizeSpeed;

	UPROPERTY(EditAnywhere) 	//Minimum Size
	float minSize;

	UPROPERTY(EditAnywhere) 	//Maximum size
	float maxSize;

	UPROPERTY(EditAnywhere) 	//Check if the planet is reducing size
	bool reductionState;

	UPROPERTY(EditAnywhere) 	//Check if the planet is growing
	bool growthState;

	// Mesh component
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* mesh;
	
};
