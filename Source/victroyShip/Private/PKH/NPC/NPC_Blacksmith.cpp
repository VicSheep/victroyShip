// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/NPC/NPC_Blacksmith.h"

#include "PKH/Animation/NPCAnimInstance.h"
#include "PKH/NPC/NPCController.h"

#define HOUR_GO_WORK 9
#define HOUR_BACK_HOME 16

ANPC_Blacksmith::ANPC_Blacksmith()
{
	NPCType = ENPCType::Chunsik;

	HomeLoc = FVector(720, -330, 88);
	WorkLoc = FVector(-220, -330, 88);

	WorkRotation = FRotator(0, 90, 0);

	static ConstructorHelpers::FObjectFinder<UAnimMontage> Montage_WorkRef(TEXT("/Script/Engine.AnimMontage'/Game/PKH/Anim/AM_BlacksmithWork.AM_BlacksmithWork'"));
	if(Montage_WorkRef.Object)
	{
		Montage_Work = Montage_WorkRef.Object;
	}
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
	if(NewHour == HOUR_GO_WORK)
	{
		NPCController->MoveToTargetLoc(WorkLoc);
		NPCController->SetIsWorking(true);
		return;
	}

	if (NewHour == HOUR_BACK_HOME)
	{
		NPCController->MoveToHome();
		NPCController->SetIsWorking(false);
		AnimInstance->StopSpecificMontage(Montage_Work);
	}
}
