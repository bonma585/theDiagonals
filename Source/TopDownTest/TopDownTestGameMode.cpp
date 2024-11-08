// Copyright Epic Games, Inc. All Rights Reserved.

#include "TopDownTestGameMode.h"
#include "TopDownTestPlayerController.h"
#include "TopDownTestCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATopDownTestGameMode::ATopDownTestGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ATopDownTestPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
}