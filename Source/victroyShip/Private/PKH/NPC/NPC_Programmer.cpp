// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/NPC/NPC_Programmer.h"

#include "Components/SlateWrapperTypes.h"
#include "Kismet/GameplayStatics.h"
#include "PKH/Animation/NPCAnimInstance.h"
#include "PKH/NPC/NPCController.h"
#include "PKH/UI/EmotionUIWidget.h"

#define HOUR_GO_WORK 9
#define HOUR_GO_PARK 11
#define HOUR_BACK_HOME 17

ANPC_Programmer::ANPC_Programmer()
{
	NPCType = ENPCType::Programmer;

	HomeLoc = FVector(2822, 6320, 1255); 
	WorkLoc = FVector(1332, 7200, 1193); 
	ParkLoc = FVector(252, 3300, 627);

	WorkRotation = FRotator();

	static ConstructorHelpers::FObjectFinder<UAnimMontage> Montage_WorkRef(TEXT("/Script/Engine.AnimMontage'/Game/PKH/Anim/AM_ProgrammerWork.AM_ProgrammerWork'"));
	if (Montage_WorkRef.Object)
	{
		Montage_Work = Montage_WorkRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> Montage_StandUpRef(TEXT("/Script/Engine.AnimMontage'/Game/PKH/Anim/AM_StandUp.AM_StandUp'"));
	if (Montage_StandUpRef.Object)
	{
		Montage_StandUp = Montage_StandUpRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> Montage_ConvRef(TEXT("/Script/Engine.AnimMontage'/Game/PKH/Anim/AM_Listen2.AM_Listen2'"));
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

		if(MyGameMode->GetHour() >= HOUR_BACK_HOME)
		{
			NPCController->MoveToHome();
		}
		else
		{
			NPCController->MoveToTargetLoc(ParkLoc);
		}
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
			UGameplayStatics::PlaySound2D(GetWorld(), Sfx_Notice);
		}
	}
	else
	{
		if (Sfx_Emotion)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), Sfx_Emotion);
		}
		if (false == NPCController->GetIsWorking())
		{
			AnimInstance->PlayMontage_Emotion(CurEmotion);
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

		NPCController->MoveToTargetLoc(ParkLoc);
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
	if(Montage != Montage_StandUp)
	{
		return;
	}
	
	NPCController->SetIsWorking(false); 
}
