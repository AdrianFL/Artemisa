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
	static ConstructorHelpers::FObjectFinder<USoundBase> FireAudio(TEXT("/Game/TwinStick/Audio/TwinStickFire.TwinStickFire"));
	FireSound = FireAudio.Object;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when ship does
	CameraBoom->TargetArmLength = 1200.f;
	CameraBoom->RelativeRotation = FRotator(-80.f, 0.f, 0.f);
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	CameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	//CameraComponent->SetupAttachment(RootComponent);
	CameraComponent->bUsePawnControlRotation = false;	// Camera does not rotate relative to arm
	//CameraComponent->bAbsoluteRotation = true;          // Camera has to be absolute so calculations work and it doesn't rotate with the camera
	//CameraComponent->bAbsoluteLocation = true;          // Camera has to be absolute so calculations work AND IT MOVES RIGHT

	// Movement
	MoveSpeed = 1500.0f;
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
	const float RightValue = GetInputAxisValue(MoveRightBinding) * 2.5f;

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
	CameraBoom->SetRelativeLocation(FVector(0.0f, 0.0f, 1.0f) * planet->GetActorScale3D().X * camera_ratio - FVector(horizontal_distance_camera, 0.0f, 0.0f));

	//Rotation
	FRotator lookAt = UKismetMathLibrary::FindLookAtRotation(CameraBoom->GetComponentLocation(), GetActorLocation());
	CameraBoom->SetRelativeRotation(lookAt);
}