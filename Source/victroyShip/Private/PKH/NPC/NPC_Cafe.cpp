// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/NPC/NPC_Cafe.h"

#include "PKH/Animation/NPCAnimInstance.h"
#include "PKH/NPC/NPCController.h"

#define HOUR_GO_WORK 10
#define HOUR_BACK_HOME 16

ANPC_Cafe::ANPC_Cafe()
{
	NPCType = ENPCType::Cafe;

	HomeLoc = FVector(1455, 4502, 632);
	WorkLoc = FVector(2220, 4140, 632);

	WorkRotation = FRotator(0, -60, 0);

	PreferItemName = TEXT("Cacao,Watermelon");

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

	// Portraits
	// Joy
	for (int i = 1; i < 3; ++i)
	{
		FString RefText = FString::Printf(TEXT("/Script/Engine.Texture2D'/Game/PKH/Portaraits/Cafe/Cafe_Joy%d.Cafe_Joy%d'"), i, i);
		ConstructorHelpers::FObjectFinder<UTexture2D> Portrait_JoyRef(*RefText);
		if (Portrait_JoyRef.Object)
		{
			Portraits_Joy.Add(Portrait_JoyRef.Object);
		}
	}
	// Surprise
	for (int i = 1; i < 3; ++i)
	{
		FString RefText = FString::Printf(TEXT("/Script/Engine.Texture2D'/Game/PKH/Portaraits/Cafe/Cafe_Surprise%d.Cafe_Surprise%d'"), i, i);
		ConstructorHelpers::FObjectFinder<UTexture2D> Portrait_SurpriseRef(*RefText);
		if (Portrait_SurpriseRef.Object)
		{
			Portraits_Surprise.Add(Portrait_SurpriseRef.Object);
		}
	}
	// Sad
	for (int i = 1; i < 3; ++i)
	{
		FString RefText = FString::Printf(TEXT("/Script/Engine.Texture2D'/Game/PKH/Portaraits/Cafe/Cafe_Sad%d.Cafe_Sad%d'"), i, i);
		ConstructorHelpers::FObjectFinder<UTexture2D> Portrait_SadRef(*RefText);
		if (Portrait_SadRef.Object)
		{
			Portraits_Sadness.Add(Portrait_SadRef.Object);
		}
	}
	// Anger
	for (int i = 1; i < 3; ++i)
	{
		FString RefText = FString::Printf(TEXT("/Script/Engine.Texture2D'/Game/PKH/Portaraits/Cafe/Cafe_Anger%d.Cafe_Anger%d'"), i, i);
		ConstructorHelpers::FObjectFinder<UTexture2D> Portrait_AngerRef(*RefText);
		if (Portrait_AngerRef.Object)
		{
			Portraits_Anger.Add(Portrait_AngerRef.Object);
		}
	}
	// Default
	for (int i = 1; i < 5; ++i)
	{
		FString RefText = FString::Printf(TEXT("/Script/Engine.Texture2D'/Game/PKH/Portaraits/Cafe/Cafe_Default%d.Cafe_Default%d'"), i, i);
		ConstructorHelpers::FObjectFinder<UTexture2D> Portrait_DefaultRef(*RefText);
		if (Portrait_DefaultRef.Object)
		{
			Portraits_Default.Add(Portrait_DefaultRef.Object);
		}
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
		return;
	}

	if(NewHour == HOUR_BACK_HOME)
	{
		NPCController->MoveToHome();
		NPCController->SetIsWorking(false);
		AnimInstance->StopSpecificMontage(Montage_Work);
	}
}
