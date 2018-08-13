// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemySpawn.h"
#include "EnemyChaser.h"
#include "Kismet/KismetMathLibrary.h"
#include "UObject/ConstructorHelpers.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/CollisionProfile.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Sound/SoundBase.h"


// Sets default values
AEnemySpawn::AEnemySpawn()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Distance of the spawn to the earth
	spawn_distance_ratio = 13.f;
	enemy_distance_ratio = 14.f;
	spawn_frecuency = 1.f;
	enemies_to_spawn = 1;
	remaining_time = spawn_frecuency;
	checkIfSpawn = true;

	// Create the mesh component
	SpawnMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpawnMesh"));
	RootComponent = SpawnMeshComponent;
}

// Called when the game starts or when spawned
void AEnemySpawn::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AEnemySpawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (enemies_to_spawn > 0)
	{
		Spawn();
	}

	//============================================================
	//COLOCATE PLATFORM
	//============================================================

	//Get surface normal and location
	FVector surfaceNormal = GetActorLocation() - planet->GetActorLocation();      //Actor location - planet location and normalize the vector
	surfaceNormal.Normalize();

	//Normal from surface * size of planet + actual location
	FVector surfaceLocation = surfaceNormal * planet->GetActorScale3D().X * spawn_distance_ratio + planet->GetActorLocation();

	//Calculate new rotation of the Artemisa
	FRotator rotation_in_surface = UKismetMathLibrary::MakeRotFromZX(surfaceNormal, GetActorForwardVector());

	//Set new rotation and location
	SetActorLocationAndRotation(surfaceLocation, rotation_in_surface);
}

void AEnemySpawn::Spawn()
{
	//Check if you can spawn
	if (checkIfSpawn)
	{

		//Variables
		FVector SpawnLocation = GetActorLocation();
		FRotator SpawnRotation = GetActorLocation().Rotation();

		if (planet)
		{
			//Get surface normal and location
			FVector surfaceNormal = GetActorLocation() - planet->GetActorLocation();      //Actor location - planet location and normalize the vector
			surfaceNormal.Normalize();

			//Normal from surface * size of planet + actual location
			FVector surfaceLocation = surfaceNormal * planet->GetActorScale3D().X * enemy_distance_ratio + planet->GetActorLocation();

			//Calculate new rotation of the Artemisa
			FRotator rotation_in_surface = UKismetMathLibrary::MakeRotFromZX(surfaceNormal, GetActorForwardVector());

			//assign to previous variables
			SpawnLocation = surfaceLocation;
			SpawnRotation = rotation_in_surface;
		}

		//Do the spawn
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			// spawn the projectile
			AEnemyChaser* enemy = World->SpawnActor<AEnemyChaser>(SpawnLocation, SpawnRotation);
			enemy->planet = planet;
			enemy->player = player;

			World->GetTimerManager().SetTimer(TimerHandle_ShotTimerExpired, this, &AEnemySpawn::SpawnTimerRenew, spawn_frecuency);

			//Set spawn to false
			checkIfSpawn = false;

			enemies_to_spawn--;
		}

		// try and play the sound if specified
		/*if (FireSound != nullptr)
		{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		}*/
	}

}

void AEnemySpawn::SpawnMoreEnemies(int s)
{
	enemies_to_spawn += s;
}

void AEnemySpawn::SpawnTimerRenew()
{
	checkIfSpawn = true;
}

