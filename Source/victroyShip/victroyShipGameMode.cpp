// Copyright Epic Games, Inc. All Rights Reserved.

#include "victroyShipGameMode.h"
#include "victroyShipCharacter.h"
#include "UObject/ConstructorHelpers.h"

AvictroyShipGameMode::AvictroyShipGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
