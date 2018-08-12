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
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ShipMesh(TEXT("/Game/TwinStick/Meshes/TwinStickUFO.TwinStickUFO"));
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
	CameraComponent->bUsePawnControlRotation = false;	// Camera does not rotate relative to arm

	// Movement
	MoveSpeed = 1000.0f;
	RotateSpeed = 1.5f;
	// Weapon
	GunOffset = FVector(90.f, 0.f, 0.f);
	FireRate = 0.1f;
	bCanFire = true;

	CurrentMovement = FVector(1.f, 0.f, 0.f);

	//Planet
	position_ratio = 13.0f;
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
	// Find movement direction
	const float ForwardValue = GetInputAxisValue(MoveForwardBinding);
	const float RightValue = GetInputAxisValue(MoveRightBinding);


	CurrentMovement = CurrentMovement.RotateAngleAxis(RotateSpeed * RightValue * ForwardValue, FVector(0.f, 0.f, 1.f));

	// Calculate  movement
	FVector Movement = CurrentMovement * MoveSpeed * DeltaSeconds * ForwardValue;

	if (RightValue != 0.f && ForwardValue == 0.f)
	{
		CurrentMovement = CurrentMovement.RotateAngleAxis(RotateSpeed * RightValue, FVector(0.f, 0.f, 1.f));
	}
	
	// If non-zero size, move this actor
	if (Movement.SizeSquared() > 0.0f)
	{
		FRotator NewRotation = Movement.Rotation();
		if (ForwardValue < 0.f)
		{
			//We rotate the mesh 180 degrees when moving backwards
			NewRotation.Add(0.f, 180.f, 0.f);
			NewRotation.Clamp();
		}
		FHitResult Hit(1.f);
		RootComponent->MoveComponent(Movement, NewRotation, true, &Hit);
		if (Hit.IsValidBlockingHit())
		{
			const FVector Normal2D = Hit.Normal.GetSafeNormal2D();
			const FVector Deflection = FVector::VectorPlaneProject(Movement, Normal2D) * (1.f - Hit.Time);
			RootComponent->MoveComponent(Deflection, NewRotation, true);
		}
		
	}
	else if (RightValue != 0.f && ForwardValue == 0.f)
	{
		FRotator NewRotation = CurrentMovement.Rotation();
		FHitResult Hit(1.f);
		RootComponent->MoveComponent(Movement, NewRotation, true, &Hit);
		if (Hit.IsValidBlockingHit())
		{
			const FVector Normal2D = Hit.Normal.GetSafeNormal2D();
			const FVector Deflection = FVector::VectorPlaneProject(Movement, Normal2D) * (1.f - Hit.Time);
			RootComponent->MoveComponent(Deflection, NewRotation, true);
		}
	}


	//=========================================
	//Set position on the surface of the planet
	//=========================================
	//Get surface normal and location
	FVector surfaceNormal = GetActorLocation() - planet->GetActorLocation();      //Actor location - planet location and normalize the vector
	surfaceNormal.Normalize();

	FVector surfaceLocation = surfaceNormal * planet->GetActorScale3D().X * position_ratio + planet->GetActorLocation(); //Normal from surface * size of planet + actual location

	//Calculate new rotation of the Artemisa
	FRotator rotation_in_surface = UKismetMathLibrary::MakeRotFromZX(surfaceNormal, GetActorForwardVector());

	//Set new rotation and location
	SetActorLocationAndRotation(surfaceLocation, rotation_in_surface);
	//========================================


	// Obtain fire input (it's really dirty using an axis, but useful and fast now)
	const float FireForwardValue = GetInputAxisValue(FireForwardBinding);;

	// Try and fire a shot
	if (FireForwardValue > 0.f)
	{
		FireShot(CurrentMovement);
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
				World->SpawnActor<AArtemisaProjectile>(SpawnLocation, FireRotation);
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
	const float RightValue = GetInputAxisValue(MoveRightBinding);

	//Calculate current movement vector
	CurrentMovement = deltaTime * MoveSpeed * FVector(ForwardValue, RightValue, 0.0f).GetClampedToSize(0.0f, 1.0f);

	//Add to the actor
	AddActorLocalOffset(CurrentMovement);

	//Get surface normal and location
	FVector surfaceNormal = GetActorLocation() - planet->GetActorLocation();      //Actor location - planet location and normalize the vector
	surfaceNormal.Normalize();

	FVector surfaceLocation = surfaceNormal * planet->GetActorScale3D().X * position_ratio + planet->GetActorLocation(); //Normal from surface * size of planet + actual location

																														 //Calculate new rotation of the Artemisa
	FRotator rotation_in_surface = UKismetMathLibrary::MakeRotFromZX(surfaceNormal, GetActorForwardVector());

	//Set new rotation and location
	SetActorLocationAndRotation(surfaceLocation, rotation_in_surface);
}