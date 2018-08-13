// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Planet.h"
#include "ArtemisaPawn.h"
#include "Components/SphereComponent.h"
#include "EnemyChaser.generated.h"

UCLASS()
class ARTEMISA_API AEnemyChaser : public AActor
{
	GENERATED_BODY()
	
	/* The mesh component */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ShipMeshComponent;
public:	
	// Sets default values for this actor's properties
	AEnemyChaser();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Function that destroys the enemy
	UFUNCTION()
	void ActivateDestruction();

	//CIRCULAR MOVEMENT VARIABLES
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Planet)
	APlanet* planet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Planet)
	AArtemisaPawn* player;
	
	/** Sphere collision component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Projectile, meta = (AllowPrivateAccess = "true"))
		USphereComponent* collisionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Planet)
		float collisionRadius;
};
