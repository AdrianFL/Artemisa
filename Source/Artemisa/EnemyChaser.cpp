// Fill out your copyright notice in the Description page of Project Settings.

#include "Kismet/KismetMathLibrary.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "Engine/StaticMesh.h"
#include "EnemyChaser.h"


// Sets default values
AEnemyChaser::AEnemyChaser()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ShipMesh(TEXT("/Game/StarterContent/enemy1.enemy1"));
	// Create the mesh component
	ShipMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	RootComponent = ShipMeshComponent;
	ShipMeshComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	ShipMeshComponent->SetStaticMesh(ShipMesh.Object);

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemyChaser::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyChaser::Tick(float DeltaTime)
{
	FVector CurrentMovement = player->GetActorLocation() - GetActorLocation();
	float distance = FVector::Dist(player->GetActorLocation(), GetActorLocation());
	
	if (distance < 10.f)
	{
		//Call to lose action
	}

	FVector Movement = FVector(1.f, 0.f, 0.f) * 1000.f * DeltaTime;

	//Add to the actor
	AddActorLocalOffset(Movement);

	//Get surface normal and location
	FVector surfaceNormal = GetActorLocation() - planet->GetActorLocation();      //Actor location - planet location and normalize the vector
	surfaceNormal.Normalize();

	//Normal from surface * size of planet + actual location
	FVector surfaceLocation = surfaceNormal * planet->GetActorScale3D().X * 13.f + planet->GetActorLocation(); 

	//Calculate new rotation of the Artemisa
	FRotator rotation_in_surface = UKismetMathLibrary::MakeRotFromZX(surfaceNormal, GetActorForwardVector());

	SetActorLocationAndRotation(surfaceLocation, rotation_in_surface);
	
	if (distance < sqrt(2.f)*planet->GetActorScale().X * 13.f) {
		RootComponent->SetRelativeRotation(FRotator(GetActorRotation().Pitch, CurrentMovement.Rotation().Yaw, GetActorRotation().Roll));
	}

}

