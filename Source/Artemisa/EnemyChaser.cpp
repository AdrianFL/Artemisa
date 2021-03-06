// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyChaser.h"
#include "Kismet/KismetMathLibrary.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "Engine/StaticMesh.h"


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

	//Create collision collider
	collisionRadius = 20.f;
	collisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("EnemyChaserCollision0"));
	collisionComponent->SetSphereRadius(collisionRadius);
	collisionComponent->SetupAttachment(ShipMeshComponent);
	collisionComponent->SetGenerateOverlapEvents(true);
}

// Called when the game starts or when spawned
void AEnemyChaser::BeginPlay()
{
	Super::BeginPlay();

	//The internet said so
	collisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemyChaser::OnBeingOverlaped);
}

// Called every frame
void AEnemyChaser::Tick(float DeltaTime)
{
	FVector CurrentMovement = player->GetActorLocation() - GetActorLocation();
	float distance = FVector::Dist(player->GetActorLocation(), GetActorLocation());

	FVector Movement = FVector(1.f, 0.f, 0.f) * 800.f * DeltaTime;

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

void AEnemyChaser::ActivateDestruction()
{
	//Activate some spectacular animations and special effects here

	//Destroy entity itself
	Destroy();
}

void AEnemyChaser::OnBeingOverlaped(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//Si hay otro actor, si no es �l mismo, y si existe el otro componente sobre el que se ha chocado
	if (OtherActor && (OtherActor != this) && OtherComp && OtherActor->IsA(AArtemisaPawn::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("DOLORAZO DE MUERTE Q ME HA MATAO Q PASA"));
		//ACTIVATE END FUCKING GAME
		player->Defeat = true;
	}
}