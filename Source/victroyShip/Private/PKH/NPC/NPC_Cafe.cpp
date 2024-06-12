// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/NPC/NPC_Cafe.h"

#include "PKH/Animation/NPCAnimInstance.h"
#include "PKH/NPC/NPCController.h"

#define HOUR_GO_WORK 10
#define HOUR_BACK_HOME 17

ANPC_Cafe::ANPC_Cafe()
{
	NPCType = ENPCType::Cafe;

	HomeLoc = FVector(1280, 5010, 634);
	WorkLoc = FVector(2220, 4140, 632);

	WorkRotation = FRotator(0, -60, 0);

	// Mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/Scanned3DPeoplePack/RP_Character/rp_eric_rigged_001_ue4/rp_eric_rigged_001_ue4.rp_eric_rigged_001_ue4'"));
	if (MeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(MeshRef.Object);
	}

	// AnimInstance
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimRef(TEXT("/Game/PKH/Anim/NPC_Cafe/ABP_Cafe.ABP_Cafe_C"));
	if (AnimRef.Class)
	{
		GetMesh()->SetAnimClass(AnimRef.Class);
	}

	// Montages
	static ConstructorHelpers::FObjectFinder<UAnimMontage> Montage_WorkRef(TEXT("/Script/Engine.AnimMontage'/Game/PKH/Anim/NPC_Cafe/AM_CafeWork.AM_CafeWork'"));
	if (Montage_WorkRef.Object)
	{
		Montage_Work = Montage_WorkRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> Montage_ConvRef(TEXT("/Script/Engine.AnimMontage'/Game/PKH/Anim/NPC_Cafe/AM_Listen2.AM_Listen2'"));
	if (Montage_ConvRef.Object)
	{
		Montage_Conv = Montage_ConvRef.Object;
	}
}

void ANPC_Cafe::BeginPlay()
{
	Super::BeginPlay();

	if (Montage_Conv)
	{
		AnimInstance->SetMontage_Conv(Montage_Conv);
	}
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
		SetNPCWalk();
		return;
	}

	if(NewHour == HOUR_BACK_HOME)
	{
		NPCController->MoveToHome();
		NPCController->SetIsWorking(false);
		AnimInstance->StopSpecificMontage(Montage_Work);
		SetNPCWalk();
	}
}
