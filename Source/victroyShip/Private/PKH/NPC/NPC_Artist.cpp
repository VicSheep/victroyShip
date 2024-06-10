// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/NPC/NPC_Artist.h"

#include "Components/SlateWrapperTypes.h"
#include "Kismet/GameplayStatics.h"
#include "PKH/Animation/NPCAnimInstance.h"
#include "PKH/NPC/NPCController.h"
#include "PKH/UI/EmotionUIWidget.h"

#define HOUR_GO_HILL 11
#define HOUR_GO_PARK 14
#define HOUR_BACK_HOME 17

ANPC_Artist::ANPC_Artist()
{
	NPCType = ENPCType::Artist;

	HomeLoc = FVector(-6606, -795, -148);
	HillLoc = FVector(-2326, -485, 628);
	ParkLoc = FVector(853, 2724, 628);

	WorkRotation = FRotator(0, 270, 0);

	static ConstructorHelpers::FObjectFinder<UAnimMontage> Montage_WorkRef(TEXT("/Script/Engine.AnimMontage'/Game/PKH/Anim/AM_ArtistWork.AM_ArtistWork'"));
	if (Montage_WorkRef.Object)
	{
		Montage_Work = Montage_WorkRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> Montage_StandUpRef(TEXT("/Script/Engine.AnimMontage'/Game/PKH/Anim/AM_StandUp.AM_StandUp'"));
	if (Montage_StandUpRef.Object)
	{
		Montage_StandUp = Montage_StandUpRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> Montage_ConvRef(TEXT("/Script/Engine.AnimMontage'/Game/PKH/Anim/AM_Listen1.AM_Listen1'"));
	if (Montage_ConvRef.Object)
	{
		Montage_Conv = Montage_ConvRef.Object;
	}
}

void ANPC_Artist::BeginPlay()
{
	Super::BeginPlay();

	if(Montage_Conv)
	{
		AnimInstance->SetMontage_Conv(Montage_Conv);
	}

	AnimInstance->OnMontageEnded.AddDynamic(this, &ANPC_Artist::OnStandUpEnded);
}

void ANPC_Artist::DoJob()
{
	Super::DoJob();

	
}

#pragma region override
void ANPC_Artist::StartConversation()
{
	if (ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
	{
		// Turn to player
		FVector DirectionVec = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		DirectionVec.Z = 0;
		const FRotator TargetRot = DirectionVec.ToOrientationRotator();
		SetActorRotation(TargetRot);

		NPCController->StartConversation();
		if (false == NPCController->GetIsWorking())
		{
			AnimInstance->PlayMontage_Conv();
		}
		EmotionUI->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ANPC_Artist::EndConversation()
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

void ANPC_Artist::OnConversationEnd()
{
	if (NPCController->GetIsWorking() && MyGameMode->GetHour() >= HOUR_BACK_HOME)
	{
		AnimInstance->StopSpecificMontage(Montage_Work);
		AnimInstance->PlayMontage_Custom(Montage_StandUp);
		NPCController->MoveToHome();
	}
}

void ANPC_Artist::PlayEmotion(bool IsUIOnly)
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

void ANPC_Artist::OnHourUpdated(int32 NewHour)
{
	if(NewHour == HOUR_GO_HILL)
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

	if(NewHour == HOUR_BACK_HOME)
	{
		if(false == NPCController->IsInConversation())
		{
			NPCController->MoveToHome();
			AnimInstance->StopSpecificMontage(Montage_Work);
			AnimInstance->PlayMontage_Custom(Montage_StandUp);
		}
	}
}

void ANPC_Artist::OnStandUpEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if(Montage != Montage_StandUp)
	{
		return;
	}

	NPCController->SetIsWorking(false);
}
