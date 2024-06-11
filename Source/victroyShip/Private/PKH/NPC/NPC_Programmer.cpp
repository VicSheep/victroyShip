// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/NPC/NPC_Programmer.h"

#include "Components/AudioComponent.h"
#include "Components/SlateWrapperTypes.h"
#include "Kismet/GameplayStatics.h"
#include "PKH/Animation/NPCAnimInstance.h"
#include "PKH/NPC/NPCController.h"
#include "PKH/UI/EmotionUIWidget.h"

#define HOUR_GO_WORK 9
#define HOUR_GO_PARK 15
#define HOUR_BACK_HOME 17

ANPC_Programmer::ANPC_Programmer()
{
	NPCType = ENPCType::Programmer;

	HomeLoc = FVector(-4310, 4410, 985);
	WorkLoc = FVector(1480, 3340, 631); 
	ParkLoc = FVector(-640, 2680, 520);

	WorkRotation = FRotator(0, 200, 0);

	SitDistance = -50.0f;

	// Mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/Scanned3DPeoplePack/RP_Character/rp_claudia_rigged_002_ue4/rp_claudia_rigged_002_ue4.rp_claudia_rigged_002_ue4'"));
	if (MeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(MeshRef.Object);
	}

	// AnimInstance
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimRef(TEXT("/Game/PKH/Anim/NPC_Programmer/ABP_Programmer.ABP_Programmer_C"));
	if (AnimRef.Class)
	{
		GetMesh()->SetAnimClass(AnimRef.Class);
	}

	// Montages
	static ConstructorHelpers::FObjectFinder<UAnimMontage> Montage_WorkRef(TEXT("/Script/Engine.AnimMontage'/Game/PKH/Anim/NPC_Programmer/AM_ProgrammerWork.AM_ProgrammerWork'"));
	if (Montage_WorkRef.Object)
	{
		Montage_Work = Montage_WorkRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> Montage_StandUpRef(TEXT("/Script/Engine.AnimMontage'/Game/PKH/Anim/NPC_Programmer/AM_StandUp.AM_StandUp'"));
	if (Montage_StandUpRef.Object)
	{
		Montage_StandUp = Montage_StandUpRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> Montage_ConvRef(TEXT("/Script/Engine.AnimMontage'/Game/PKH/Anim/NPC_Programmer/AM_Listen2.AM_Listen2'"));
	if (Montage_ConvRef.Object)
	{
		Montage_Conv = Montage_ConvRef.Object;
	}
}

void ANPC_Programmer::BeginPlay()
{
	Super::BeginPlay();

	AnimInstance->OnMontageEnded.AddDynamic(this, &ANPC_Programmer::OnStandUpEnded);

	if (Montage_Conv)
	{
		AnimInstance->SetMontage_Conv(Montage_Conv);
	}
}

void ANPC_Programmer::DoJob()
{
	Super::DoJob();

	
}

#pragma region override
void ANPC_Programmer::StartConversation()
{
	if (ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
	{
		// Turn to player
		FVector DirectionVec = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		DirectionVec.Z = 0;
		const FRotator TargetRot = DirectionVec.ToOrientationRotator();
		SetActorRotation(TargetRot);

		NPCController->StartConversation();
		if(false == NPCController->GetIsWorking())
		{
			AnimInstance->PlayMontage_Conv();
		}
		EmotionUI->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ANPC_Programmer::EndConversation()
{
	EmotionUI->SetVisibility(ESlateVisibility::Hidden);
	NPCController->EndConversation();
	if (false == NPCController->GetIsWorking())
	{
		AnimInstance->StopAllMontages(0);
	}
	SetNPCWalk();

	OnConversationEnd();
}

void ANPC_Programmer::OnConversationEnd()
{
	if(NPCController->GetIsWorking() && MyGameMode->GetHour() >= HOUR_GO_PARK)
	{
		AnimInstance->StopSpecificMontage(Montage_Work);
		AnimInstance->PlayMontage_Custom(Montage_StandUp);
	}
}

void ANPC_Programmer::PlayEmotion(bool IsUIOnly)
{
	if (CurEmotion.IsEmpty())
	{
		return;
	}

	EmotionUI->SetEmotion(CurEmotion);
	EmotionUI->SetVisibility(ESlateVisibility::Visible);
	if (IsUIOnly)
	{
		if (Sfx_Notice)
		{
			SfxComp->SetSound(Sfx_Notice);
			SfxComp->Play();
		}
	}
	else
	{
		if (false == NPCController->GetIsWorking())
		{
			AnimInstance->PlayMontage_Emotion(CurEmotion);
		}

		if (CurEmotion == "joy" && IsValid(Sfx_Joy))
		{
			SfxComp->SetSound(Sfx_Joy);
		}
		else if (CurEmotion == "anger" && IsValid(Sfx_Anger))
		{
			SfxComp->SetSound(Sfx_Anger);
		}
		else if (CurEmotion == "sadness" && IsValid(Sfx_Sad))
		{
			SfxComp->SetSound(Sfx_Sad);
		}
		else if (CurEmotion == "surprise" && IsValid(Sfx_Surprise))
		{
			SfxComp->SetSound(Sfx_Surprise);
		}
		else if (IsValid(Sfx_Indiff))
		{
			SfxComp->SetSound(Sfx_Indiff);
		}

		if (SfxComp->Sound)
		{
			SfxComp->Play();
		}
	}
}
#pragma endregion

void ANPC_Programmer::OnHourUpdated(int32 NewHour)
{
	if (NewHour == HOUR_GO_WORK)
	{
		NPCController->MoveToTargetLoc(WorkLoc);
		NPCController->SetIsWorking(true);
		return;
	}

	if (NewHour == HOUR_GO_PARK)
	{
		if (NPCController->IsInConversation())
		{
			return;
		}

		AnimInstance->StopSpecificMontage(Montage_Work);
		AnimInstance->PlayMontage_Custom(Montage_StandUp);
		return;
	}

	if (NewHour == HOUR_BACK_HOME)
	{
		NPCController->MoveToHome();
	}
}

void ANPC_Programmer::OnStandUpEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if(Montage == Montage_StandUp)
	{
		NPCController->SetIsWorking(false);
		NPCController->MoveToTargetLoc(ParkLoc);
	}
}
