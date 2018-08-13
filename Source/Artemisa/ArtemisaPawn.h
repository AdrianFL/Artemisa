// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Planet.h"
#include "ArtemisaProjectile.h"
#include "ArtemisaPawn.generated.h"

UCLASS(Blueprintable)
class AArtemisaPawn : public APawn
{
	GENERATED_BODY()

	/* The mesh component */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ShipMeshComponent;

	/** Spring arm that will offset the camera */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* SpringArm;

	/** Camera component that will be our viewpoint */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* Camera;

public:
	AArtemisaPawn();

	/** Offset from the ships location to spawn projectiles */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite )
	FVector GunOffset;
	
	/* How fast the weapon will fire */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	float FireRate;

	/* The speed our ship moves around the level */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	float MoveSpeed;

	/* The rotation speed of our ship */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	float RotateSpeed;
	
	/** Sound to play each time we fire */
	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
	class USoundBase* FireSound;

	/* Win and lose conditions */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	bool Victory = false;
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	bool Defeat = false;

	// Begin Actor Interface
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End Actor Interface

	/* Fire a shot in the specified direction */
	void FireShot(FVector FireDirection);

	/* Handler for the fire timer expiry */
	void ShotTimerExpired();

	// Static names for axis bindings
	static const FName MoveForwardBinding;
	static const FName MoveRightBinding;
	static const FName FireForwardBinding;
	static const FName FireRightBinding;

	//CIRCULAR MOVEMENT VARIABLES
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Planet)
		APlanet* planet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Planet)
		float position_ratio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Planet)
		float camera_ratio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Planet)
		float horizontal_distance_camera;

	void MoveOnPlanet(float); //Move the artemisa ship on the planet

	/** Sphere collision component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Projectile, meta = (AllowPrivateAccess = "true"))
	USphereComponent* collisionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Planet)
	float collisionRadius;

private:

	/* Flag to control firing  */
	uint32 bCanFire : 1;

	/** Handle for efficient management of ShotTimerExpired timer */
	FTimerHandle TimerHandle_ShotTimerExpired;

	/* Current vector of pawn's movement */
	FVector CurrentMovement;

public:
	/** Returns ShipMeshComponent subobject **/
	FORCEINLINE class UStaticMeshComponent* GetShipMeshComponent() const { return ShipMeshComponent; }
	/** Returns CameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetCameraComponent() const { return Camera; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return SpringArm; }
};

