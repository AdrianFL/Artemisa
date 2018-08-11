// Fill out your copyright notice in the Description page of Project Settings.

#include "ArtemisaPlayerController.h"


void AArtemisaPlayerController::BeginPlay()
{
	Super::BeginPlay();
	SetInputMode(FInputModeGameAndUI());
}

