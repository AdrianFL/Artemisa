// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ArtemisaGameMode.h"
#include "ArtemisaPawn.h"

AArtemisaGameMode::AArtemisaGameMode()
{
	// set default pawn class to our character class
	DefaultPawnClass = AArtemisaPawn::StaticClass();
}

void AArtemisaGameMode::BeginPlay()
{
	Super::BeginPlay();
	ChangeMenuWidget(StartingWidgetClass);
}

void AArtemisaGameMode::ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass)
{
	if (CurrentWidget != nullptr)
	{
		CurrentWidget->RemoveFromViewport();
		CurrentWidget = nullptr;
	}
	if (NewWidgetClass != nullptr)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), NewWidgetClass);
		if (CurrentWidget != nullptr)
		{
			CurrentWidget->AddToViewport();
		}
	}
}