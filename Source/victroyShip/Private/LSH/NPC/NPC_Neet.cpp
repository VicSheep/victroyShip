// Fill out your copyright notice in the Description page of Project Settings.


#include "LSH/NPC/NPC_Neet.h"

#include "PKH/Animation/NPCAnimInstance.h"
#include "PKH/NPC/NPCController.h"

#define HOUR_SLEEP 9
#define HOUR_GO_PARK 11
#define HOUR_BACK_HOME 14

ANPC_Neet::ANPC_Neet()
{
	NPCType = ENPCType::Unemployed;//юс╫ц

	HomeLoc = FVector(1450, -2180, 88);
	HillLoc = FVector(210, -3490, 88);
	ParkLoc = FVector(-980, -1410, 88);

	WorkRotation = FRotator(0, 270, 0);

	static ConstructorHelpers::FObjectFinder<UAnimMontage> Montage_WorkRef(TEXT("/Script/Engine.AnimMontage'/Game/PKH/Anim/AM_ArtistWork.AM_ArtistWork'"));
	if (Montage_WorkRef.Object)
	{
		Montage_Work = Montage_WorkRef.Object;
	}
}

void ANPC_Neet::BeginPlay()
{
	Super::BeginPlay();


}

void ANPC_Neet::DoJob()
{
	Super::DoJob();


}

void ANPC_Neet::OnHourUpdated(int32 NewHour)
{
	if (NewHour == HOUR_SLEEP)
	{
		Montage_Work = Montage_Sleep;
		NPCController->MoveToTargetLoc(HomeLoc);
		NPCController->SetIsWorking(true);
		return;
	}

	if (NewHour == HOUR_GO_PARK)
	{
		AnimInstance->StopSpecificMontage(Montage_Work);
		Montage_Work = Montage_Sit;
		ParkLoc = FVector(-3270, -380, 541);
		WorkRotation = FRotator(0, -80, 0);
		NPCController->MoveToTargetLoc(ParkLoc);
		NPCController->SetIsWorking(true);
		return;
	}

	if (NewHour == HOUR_BACK_HOME)
	{
		AnimInstance->StopSpecificMontage(Montage_Work);
		Montage_Work = Montage_Game;
		WorkRotation = FRotator(0, -80, 0);
		NPCController->MoveToTargetLoc(HomeLoc);
		NPCController->SetIsWorking(true);
	}
}