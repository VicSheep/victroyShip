// Fill out your copyright notice in the Description page of Project Settings.


#include "OJS/New/OJSGameModeBase.h"
#include "UObject/ConstructorHelpers.h"

AOJSGameModeBase::AOJSGameModeBase()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/OJS/NEW/BP_OJSplayCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
