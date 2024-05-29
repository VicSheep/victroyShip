// Fill out your copyright notice in the Description page of Project Settings.


#include "OJS/Game/FarmLifeOjsGameModeBase.h"
#include "UObject/ConstructorHelpers.h"

AFarmLifeOjsGameModeBase::AFarmLifeOjsGameModeBase()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/OJS/Player/BP_FarmLifeOjsPlayerCharacter"));
	if (PlayerPawnBPClass.Class != NULL)                      
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
