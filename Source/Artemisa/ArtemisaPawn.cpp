// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ArtemisaPawn.h"
#include "ArtemisaProjectile.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/CollisionProfile.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Sound/SoundBase.h"

const FName AArtemisaPawn::MoveForwardBinding("MoveForward");
const FName AArtemisaPawn::MoveRightBinding("MoveRight");
const FName AArtemisaPawn::FireForwardBinding("FireForward");
const FName AArtemisaPawn::FireRightBinding("FireRight");

AArtemisaPawn::AArtemisaPawn()
{	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ShipMesh(TEXT("/Game/StarterContent/spaceship.spaceship"));
	// Create the mesh component
	ShipMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	RootComponent = ShipMeshComponent;
	ShipMeshComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	ShipMeshComponent->SetStaticMesh(ShipMesh.Object);
	
	// Cache our sound effect
	static ConstructorHelpers::FObjectFinder<USoundBase> FireAudio(TEXT("/Game/TwinStick/Audio/shoot.shoot"));
	FireSound = FireAudio.Object;

	// Create a spring arm component
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 90));
	SpringArm->SetWorldRotation(FRotator(-90.0f, 0.0f, 0.0f));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 2000;
	SpringArm->bEnableCameraLag = false;
	SpringArm->bEnableCameraRotationLag = false;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritYaw = false;
	SpringArm->bInheritRoll = false;

	// Create camera component 
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera0"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);	// Attach the camera
	Camera->bUsePawnControlRotation = false; // Don't rotate camera with controller

	//Create collision component
	collisionRadius = 50.f;
	collisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("ArtemisaCollision0"));
	collisionComponent->SetSphereRadius(collisionRadius);
	collisionComponent->SetupAttachment(ShipMeshComponent);
	collisionComponent->SetGenerateOverlapEvents(true);

	// Movement
	MoveSpeed = 2000.0f;
	RotateSpeed = 1.5f;
	// Weapon
	GunOffset = FVector(90.f, 0.f, 0.f);
	FireRate = 0.15f;
	bCanFire = true;

	CurrentMovement = FVector(1.f, 0.f, 0.f);

	//Planet
	position_ratio = 13.0f;
	camera_ratio = 5.0f;
	horizontal_distance_camera = 200.f;
}

void AArtemisaPawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	// set up gameplay key bindings
	PlayerInputComponent->BindAxis(MoveForwardBinding);
	PlayerInputComponent->BindAxis(MoveRightBinding);
	PlayerInputComponent->BindAxis(FireForwardBinding);
	PlayerInputComponent->BindAxis(FireRightBinding);
}

void AArtemisaPawn::Tick(float DeltaSeconds)
{
	//Move surrounding the planet
	MoveOnPlanet(DeltaSeconds);
	
	// Obtain fire input (it's really dirty using an axis, but useful and fast now)
	const float FireForwardValue = GetInputAxisValue(FireForwardBinding);;

	// Try and fire a shot
	if (FireForwardValue > 0.f)
	{
		FireShot(GetActorForwardVector());
	}

	//Check if we have won
	if (planet->VictoryCondition)
	{
		Victory = true;
	}
}

void AArtemisaPawn::FireShot(FVector FireDirection)
{
	// If it's ok to fire again
	if (bCanFire == true)
	{
		// If we are pressing fire stick in a direction
		if (FireDirection.SizeSquared() > 0.0f)
		{
			const FRotator FireRotation = FireDirection.Rotation();
			// Spawn projectile at an offset from this pawn
			const FVector SpawnLocation = GetActorLocation() + FireRotation.RotateVector(GunOffset);

			UWorld* const World = GetWorld();
			if (World != NULL)
			{
				// spawn the projectile
				AArtemisaProjectile* projectile = World->SpawnActor<AArtemisaProjectile>(SpawnLocation, FireRotation);
				projectile->planet = planet;
			}

			bCanFire = false;
			World->GetTimerManager().SetTimer(TimerHandle_ShotTimerExpired, this, &AArtemisaPawn::ShotTimerExpired, FireRate);

			// try and play the sound if specified
			if (FireSound != nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
			}

			bCanFire = false;
		}
	}
}

void AArtemisaPawn::ShotTimerExpired()
{
	bCanFire = true;
}

void AArtemisaPawn::MoveOnPlanet(float deltaTime)
{
	// Find movement direction
	const float ForwardValue = GetInputAxisValue(MoveForwardBinding);
	const float RightValue = GetInputAxisValue(MoveRightBinding) * 3.f;

	//Calculate current movement vector
	FVector Movement = CurrentMovement * MoveSpeed * deltaTime * ForwardValue;

	//Add to the actor
	AddActorLocalOffset(Movement);

	//Get surface normal and location
	FVector surfaceNormal = GetActorLocation() - planet->GetActorLocation();      //Actor location - planet location and normalize the vector
	surfaceNormal.Normalize();

	FVector surfaceLocation = surfaceNormal * planet->GetActorScale3D().X * position_ratio + planet->GetActorLocation(); //Normal from surface * size of planet + actual location

	//Calculate new rotation of the Artemisa
	FRotator rotation_in_surface = UKismetMathLibrary::MakeRotFromZX(surfaceNormal, GetActorForwardVector());

	//Set new rotation and location
	SetActorLocationAndRotation(surfaceLocation, rotation_in_surface);

	//Apply rotation
	if (ForwardValue < 0.f)
	{
		RootComponent->AddLocalRotation(FRotator(ForwardValue, -RightValue, 0.0f));
	}
	else
	{
		RootComponent->AddLocalRotation(FRotator(ForwardValue, RightValue, 0.0f));
	}

	//=======================================================
	///////////////////////CAMERA////////////////////////////
	//=======================================================
	//Location
	// Calculus: "Absolute position of object" where the ship is + normal from surface * distance + (200m behind = - forward vector * 200.f * distance desired from ship)
	SpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 1.0f) * planet->GetActorScale3D().X * camera_ratio - FVector(horizontal_distance_camera, 0.0f, 0.0f));
	//FVector normal = planet->GetActorLocation() - this->GetActorLocation();
	//normal.Normalize();
	//SpringArm->SetRelativeLocation(normal * 2000);

	//Rotation
	FRotator lookAt = UKismetMathLibrary::FindLookAtRotation(SpringArm->GetComponentLocation(), GetActorLocation());
	SpringArm->SetRelativeRotation(lookAt);

	

	// ABSOLUTE

	//=======================================================
	///////////////////////CAMERA////////////////////////////
	//=======================================================
	//Location
	// Calculus: "Absolute position of object" where the ship is + normal from surface * distance + (200m behind = - forward vector * 200.f * distance desired from ship)

	/*CameraComponent->SetRelativeLocation(surfaceNormal * camera_ratio + GetActorLocation());

	//Rotation
	//FRotator lookAt = UKismetMathLibrary::FindLookAtRotation(CameraComponent->GetComponentLocation(), GetActorLocation());
	FRotator lookAt = GetActorLocation().Rotation();
	lookAt.Yaw = 0.0f;     //Fixed looking position
	//lookAt.Pitch = -lookAt.Pitch; //Reverse
	lookAt.Roll  = -lookAt.Roll;  //Reverse
	lookAt.Pitch -= 80.0f; //Looking down
	
	CameraComponent->SetRelativeRotation(lookAt);*/

}