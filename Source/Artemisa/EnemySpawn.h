// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Planet.h"
#include "ArtemisaPawn.h"
#include "Components/StaticMeshComponent.h"
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
	UFUNCTION(BlueprintCallable)
	void Spawn();

	//Add more enemies
	UFUNCTION(BlueprintCallable)
	void SpawnMoreEnemies(int s);

	//Renew spawn checker
	UFUNCTION(BlueprintCallable)
	void SpawnTimerRenew();

	//SPAWN ACTOR DATA
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawn)
	float spawn_frecuency;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawn)
	int enemies_to_spawn;

	//PLAYER DATA
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawn)
		AArtemisaPawn* player;

	//PLANET DATA
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawn)
		APlanet* planet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawn)
	float enemy_distance_ratio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawn)
	float spawn_distance_ratio;

	/** Handle for efficient management of ShotTimerExpired timer */
	FTimerHandle TimerHandle_ShotTimerExpired;

	/* The mesh component */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* SpawnMeshComponent;
};
