// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Planet.h"
#include "EnemySpawn.generated.h"

UCLASS()
class ARTEMISA_API AEnemySpawn : public AActor
{
	GENERATED_BODY()

	//Private variables
	float remaining_time;
	int remaining_enemies;
	bool checkIfSpawn;

public:	
	// Sets default values for this actor's properties
	AEnemySpawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Spawn at a specific location and a number of enemies
	void Spawn(int);
	
	//Renew spawn checker
	void SpawnTimerRenew();

	//SPAWN ACTOR DATA
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Planet)
	float spawn_frecuency;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Planet)
	int enemies_to_spawn;

	//PLANET DATA
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Planet)
		APlanet* planet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Planet)
	float enemy_distance_ratio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Planet)
	float spawn_distance_ratio;

	/** Handle for efficient management of ShotTimerExpired timer */
	FTimerHandle TimerHandle_ShotTimerExpired;
};
