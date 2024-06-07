// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/NPC/NPC_Cafe.h"

#include "PKH/Animation/NPCAnimInstance.h"
#include "PKH/NPC/NPCController.h"

#define HOUR_GO_WORK 10
#define HOUR_BACK_HOME 17

ANPC_Cafe::ANPC_Cafe()
{
	NPCType = ENPCType::Cafe;

	HomeLoc = FVector(1270, 1790, 88);
	WorkLoc = FVector(-1590, 1790, 88);

	WorkRotation = FRotator(0, 0, 0);

	static ConstructorHelpers::FObjectFinder<UAnimMontage> Montage_WorkRef(TEXT("/Script/Engine.AnimMontage'/Game/PKH/Anim/AM_CafeWork.AM_CafeWork'"));
	if (Montage_WorkRef.Object)
	{
		Montage_Work = Montage_WorkRef.Object;
	}
}

void ANPC_Cafe::BeginPlay()
{
	Super::BeginPlay();


}

void ANPC_Cafe::DoJob()
{
	Super::DoJob();

	
}

void ANPC_Cafe::OnHourUpdated(int32 NewHour)
{
	if(NewHour == HOUR_GO_WORK)
	{
		NPCController->MoveToTargetLoc(WorkLoc);
		NPCController->SetIsWorking(true);
		return;
	}

	if(NewHour == HOUR_BACK_HOME)
	{
		NPCController->MoveToHome();
		NPCController->SetIsWorking(false);
		AnimInstance->StopSpecificMontage(Montage_Work);
	}
}
