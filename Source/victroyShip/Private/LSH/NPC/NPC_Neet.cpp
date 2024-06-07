// Fill out your copyright notice in the Description page of Project Settings.


#include "LSH/NPC/NPC_Neet.h"

#include "PKH/Animation/NPCAnimInstance.h"
#include "PKH/NPC/NPCController.h"

#define HOUR_GO_HILL 9
#define HOUR_GO_PARK 14
#define HOUR_BACK_HOME 17

ANPC_Neet::ANPC_Neet()
{
	NPCType = ENPCType::Mira;//юс╫ц

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
	if (NewHour == HOUR_GO_HILL)
	{
		NPCController->MoveToTargetLoc(HillLoc);
		return;
	}

	if (NewHour == HOUR_GO_PARK)
	{
		NPCController->MoveToTargetLoc(ParkLoc);
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