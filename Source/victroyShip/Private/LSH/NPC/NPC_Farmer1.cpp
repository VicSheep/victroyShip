// Fill out your copyright notice in the Description page of Project Settings.


#include "LSH/NPC/NPC_Farmer1.h"

#include "PKH/Animation/NPCAnimInstance.h"
#include "PKH/NPC/NPCController.h"

#define HOUR_WORK 9
#define HOUR_GO_CAFE 14
#define HOUR_WORK2 15
#define HOUR_HOME 17


ANPC_Farmer1::ANPC_Farmer1()
{
	NPCType = ENPCType::Farmer;

	HomeLoc = FVector(1450, -2180, 88);
	FarmLoc = FVector(210, -3490, 88);
	CafeLoc = FVector(210, -3490, 88);

	WorkRotation = FRotator(0, 270, 0);

	static ConstructorHelpers::FObjectFinder<UAnimMontage> Montage_WorkRef(TEXT("/Script/Engine.AnimMontage'/Game/PKH/Anim/AM_ArtistWork.AM_ArtistWork'"));
	if (Montage_WorkRef.Object)
	{
		Montage_Work = Montage_WorkRef.Object;
	}
}

void ANPC_Farmer1::BeginPlay()
{
	Super::BeginPlay();


}

void ANPC_Farmer1::DoJob()
{
	Super::DoJob();


}

void ANPC_Farmer1::OnHourUpdated(int32 NewHour)
{
	if (NewHour == HOUR_WORK)
	{
		Montage_Work = Montage_FarmWork;
		NPCController->MoveToTargetLoc(FarmLoc);
		NPCController->SetIsWorking(true);
		return;
	}

	if (NewHour == HOUR_GO_CAFE)
	{
		AnimInstance->StopSpecificMontage(Montage_Work);
		Montage_Work = Montage_Sit;
		WorkRotation = FRotator(0, -80, 0);
		NPCController->MoveToTargetLoc(CafeLoc);
		NPCController->SetIsWorking(true);
		return;
	}

	if (NewHour == HOUR_WORK2)
	{
		AnimInstance->StopSpecificMontage(Montage_Work);
		Montage_Work = Montage_FarmWork;
		WorkRotation = FRotator(0, -80, 0);
		NPCController->MoveToTargetLoc(FarmLoc);
		NPCController->SetIsWorking(true);
	}

	if (NewHour == HOUR_HOME)
	{
		NPCController->MoveToHome();
		NPCController->SetIsWorking(false);
		AnimInstance->StopSpecificMontage(Montage_Work);
	}
}