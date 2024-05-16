// Fill out your copyright notice in the Description page of Project Settings.


#include "OJS/FarmLifeGameBase.h"
#include "UObject/ConstructorHelpers.h"

AFarmLifeGameBase::AFarmLifeGameBase()
{
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/OJS/player/BP_PlayerCharacter"));
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/OJS/player/BP_FarmLifePlayableCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
