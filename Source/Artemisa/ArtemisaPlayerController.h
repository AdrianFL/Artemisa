// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ArtemisaPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ARTEMISA_API AArtemisaPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	
};
