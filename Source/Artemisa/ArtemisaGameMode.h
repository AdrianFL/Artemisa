// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include <map>
#include <functional>
#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ArtemisaGameMode.generated.h"

UENUM(BlueprintType)
enum class AGameState : uint8 {
	INIT,
	PLAY,
	PAUSE,
	WIN,
	DEFEAT,
	OPTIONS
};

UCLASS(MinimalAPI)
class AArtemisaGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AArtemisaGameMode();

	/** Remove the current menu widget and create a new one from the specified class, if provided. */
	UFUNCTION(BlueprintCallable, Category = "GUI")
	void ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass);

	/** Function to change state */
	UFUNCTION(BlueprintCallable, Category = "States")
	void ChangeState(AGameState NewState);

	UFUNCTION(BlueprintCallable, Category = "States")
	AGameState GetAGameState() const { return CurrentState; }

	/* The level we are in */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	int Level = 1;

protected:
	/** Called when the game starts. */
	virtual void BeginPlay() override;

	/** The widget class we will use as our menu when the game starts. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG Game")
	TSubclassOf<UUserWidget> StartingWidgetClass;

	/** The widget instance that we are using as our menu. */
	UPROPERTY()
	UUserWidget* CurrentWidget;

private:
	//Map of functions to be called when there's an state change
	std::map<AGameState, void (AArtemisaGameMode::*)()> StateFunctions;
	AGameState CurrentState;

	UFUNCTION(BlueprintCallable, Category = "States")
	void Init();
	UFUNCTION(BlueprintCallable, Category = "States")
	void Play();
	UFUNCTION(BlueprintCallable, Category = "States")
	void Pause();
	UFUNCTION(BlueprintCallable, Category = "States")
	void Win();
	UFUNCTION(BlueprintCallable, Category = "States")
	void Defeat();
	UFUNCTION(BlueprintCallable, Category = "States")
	void Options();
};



