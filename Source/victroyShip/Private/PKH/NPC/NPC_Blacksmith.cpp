﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/NPC/NPC_Blacksmith.h"

#include "PKH/NPC/NPCController.h"

#define HOUR_OUTSIDE 9
#define HOUR_BACK_TO_HOME 10

ANPC_Blacksmith::ANPC_Blacksmith()
{
	NPCName = TEXT("이춘식");

	HomeLoc = FVector(720, -330, 88);
	TargetLoc = FVector(-220, -330, 88);
}

void ANPC_Blacksmith::BeginPlay()
{
	Super::BeginPlay();

	
}

void ANPC_Blacksmith::DoJob()
{
	Super::DoJob();


}

void ANPC_Blacksmith::OnHourUpdated(int32 NewHour)
{
	if(NewHour == HOUR_OUTSIDE)
	{
		NPCController->MoveToTargetLoc(TargetLoc); UE_LOG(LogTemp, Warning, TEXT("Chunsick Go Outside"));
		return;
	}

	if (NewHour == HOUR_BACK_TO_HOME)
	{
		NPCController->MoveToHome(); UE_LOG(LogTemp, Warning, TEXT("Chunsick Back Home"));
		return;
	}

}
