// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ArtemisaGameMode.h"
#include "ArtemisaPawn.h"

AArtemisaGameMode::AArtemisaGameMode()
{
	// set default pawn class to our character class
	DefaultPawnClass = AArtemisaPawn::StaticClass();
	
	CurrentState = AGameState::INIT;

	StateFunctions[AGameState::INIT] = &AArtemisaGameMode::Init;
	StateFunctions[AGameState::PLAY] = &AArtemisaGameMode::Play;
	StateFunctions[AGameState::PAUSE] = &AArtemisaGameMode::Pause;
	StateFunctions[AGameState::WIN] = &AArtemisaGameMode::Win;
	StateFunctions[AGameState::DEFEAT] = &AArtemisaGameMode::Defeat;
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

void AArtemisaGameMode::ChangeState(AGameState NewState)
{
	//If new state is different
	if (NewState != CurrentState) {
		//If we can find a function that changes the state
		auto iter = StateFunctions.find(NewState);
		if (iter != StateFunctions.end()) {
			void (AArtemisaGameMode::*function)() = iter->second;
			(this->*function)();
			CurrentState = NewState;
		}
	}
}

void AArtemisaGameMode::Init() {}
void AArtemisaGameMode::Play() {}
void AArtemisaGameMode::Pause() {}
void AArtemisaGameMode::Win() {}
void AArtemisaGameMode::Defeat() {}