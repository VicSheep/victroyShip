// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/NPC/NPC_Programmer.h"

#define HOUR_GO_WORK 9
#define HOUR_GO_PARK 15
#define HOUR_BACK_HOME 17

ANPC_Programmer::ANPC_Programmer()
{
	NPCType = ENPCType::Programmer;

	HomeLoc = FVector(2822, 6320, 1255); 
	WorkLoc = FVector(13332, 7200, 1193); 
	ParkLoc = FVector(252, 3300, 627);
}

void ANPC_Programmer::BeginPlay()
{
	Super::BeginPlay();


}

void ANPC_Programmer::DoJob()
{
	Super::DoJob();


}

void ANPC_Programmer::OnHourUpdated(int32 NewHour)
{

}
