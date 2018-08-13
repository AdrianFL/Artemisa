// Copyright 1998-2018 Epic Games, Inc. All Rights Reserve

#include "ArtemisaProjectile.h"
#include "EnemyChaser.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/StaticMesh.h"

AArtemisaProjectile::AArtemisaProjectile() 
{
	// Static reference to the mesh to use for the projectile
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ProjectileMeshAsset(TEXT("/Game/TwinStick/Meshes/TwinStickProjectile.TwinStickProjectile"));

	// Create mesh component for the projectile sphere
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh0"));
	ProjectileMesh->SetStaticMesh(ProjectileMeshAsset.Object);
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->BodyInstance.SetCollisionProfileName("Projectile");
	ProjectileMesh->OnComponentHit.AddDynamic(this, &AArtemisaProjectile::OnHit);		// set up a notification for when this component hits something
	RootComponent = ProjectileMesh;

	PrimaryActorTick.bCanEverTick = true;
	/*
	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement0"));
	ProjectileMovement->UpdatedComponent = ProjectileMesh;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.f; // No gravity
	*/
	// Die after 3 seconds by default
	InitialLifeSpan = 3.5f;

	//Create collision component
	collisionRadius = 20.f;
	collisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("ProjectileCollision0"));
	collisionComponent->SetSphereRadius(collisionRadius);
	collisionComponent->SetupAttachment(ProjectileMesh);
	collisionComponent->SetGenerateOverlapEvents(true);
}

void AArtemisaProjectile::Tick(float DeltaSeconds)
{
	FVector Movement = FVector(1.f, 0.f, 0.f) * 1800.f * DeltaSeconds;

	//Add to the actor
	AddActorLocalOffset(Movement);

	//Get surface normal and location
	FVector surfaceNormal = GetActorLocation() - planet->GetActorLocation();      //Actor location - planet location and normalize the vector
	surfaceNormal.Normalize();

	FVector surfaceLocation = surfaceNormal * planet->GetActorScale3D().X * 13.f + planet->GetActorLocation(); //Normal from surface * size of planet + actual location

	//Calculate new rotation of the Artemisa
	FRotator rotation_in_surface = UKismetMathLibrary::MakeRotFromZX(surfaceNormal, GetActorForwardVector());

	
	SetActorLocationAndRotation(surfaceLocation, rotation_in_surface);
}

void AArtemisaProjectile::BeginPlay()
{
	//Always do this babe
	Super::BeginPlay();

	//The internet said so
	collisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AArtemisaProjectile::OnBeingOverlaped);
}

void AArtemisaProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 20.0f, GetActorLocation());
	}

	Destroy();
}

void AArtemisaProjectile::OnBeingOverlaped(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Que pasa AQUI QUE OVERLAPEA CON NAVE"));

	//Si hay otro actor, si no es él mismo, y si existe el otro componente sobre el que se ha chocado
	/*if (OtherActor && (OtherActor != this) && OtherComp && OtherActor->IsA(AEnemyChaser::StaticClass()))
	{
		
		//dynamic_cast<AEnemyChaser*>(OtherActor)->ActivateDestruction();
		//InitialLifeSpan = 0.0f;
		//Destroy();
	}*/
}