// Fill out your copyright notice in the Description page of Project Settings.


#include "OJS/Game/TrueGameBase.h"
#include "UObject/ConstructorHelpers.h"
ATrueGameBase::ATrueGameBase()

{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/OJS/Player/BP_TruePlayerCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}