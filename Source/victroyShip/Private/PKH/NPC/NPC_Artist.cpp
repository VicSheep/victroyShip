// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/NPC/NPC_Artist.h"

#include "PKH/Animation/NPCAnimInstance.h"
#include "PKH/NPC/NPCController.h"

#define HOUR_GO_HILL 9
#define HOUR_GO_PARK 13
#define HOUR_BACK_HOME 17

ANPC_Artist::ANPC_Artist()
{
	NPCType = ENPCType::Artist;

	HomeLoc = FVector(2104, 6720, 1207);
	HillLoc = FVector(3040, 4301, 631);
	ParkLoc = FVector(1030, 1670, 537);

	WorkRotation = FRotator(0, 190, 0);

	SitDistance = -40.0f;

	PreferItemName = TEXT("Grape,Tomato");

	// Mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/Scanned3DPeoplePack/RP_Character/rp_sophia_rigged_003_ue4/rp_sophia_rigged_003_ue4.rp_sophia_rigged_003_ue4'"));
	if (MeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(MeshRef.Object);
	}

	// AnimInstance
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimRef(TEXT("/Game/PKH/Anim/NPC_Artist/ABP_Artist.ABP_Artist_C"));
	if (AnimRef.Class)
	{
		GetMesh()->SetAnimClass(AnimRef.Class);
	}


	// Montages
	static ConstructorHelpers::FObjectFinder<UAnimMontage> Montage_WorkRef(TEXT("/Script/Engine.AnimMontage'/Game/PKH/Anim/NPC_Artist/AM_ArtistWork.AM_ArtistWork'"));
	if (Montage_WorkRef.Object)
	{
		Montage_Work = Montage_WorkRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> Montage_StandUpRef(TEXT("/Script/Engine.AnimMontage'/Game/PKH/Anim/NPC_Artist/AM_StandUp.AM_StandUp'"));
	if (Montage_StandUpRef.Object)
	{
		Montage_StandUp = Montage_StandUpRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> Montage_ConvRef(TEXT("/Script/Engine.AnimMontage'/Game/PKH/Anim/NPC_Artist/AM_Listen1.AM_Listen1'"));
	if (Montage_ConvRef.Object)
	{
		Montage_Conv = Montage_ConvRef.Object;
	}

	// Object
	static ConstructorHelpers::FClassFinder<AActor> EaselClassRef(TEXT("/Game/PKH/Blueprint/BP_Easel.BP_Easel_C"));
	if(EaselClassRef.Class)
	{
		EaselClass = EaselClassRef.Class;
	}

	// Portraits
	// Joy
	for(int i = 1; i < 5; ++i)
	{
		FString RefText = FString::Printf(TEXT("/Script/Engine.Texture2D'/Game/PKH/Portaraits/Artist/Artist_Joy%d.Artist_Joy%d'"), i, i);
		static ConstructorHelpers::FObjectFinder<UTexture2D> Portrait_JoyRef(*RefText);
		if (Portrait_JoyRef.Object)
		{
			Portraits_Joy.Add(Portrait_JoyRef.Object);
		}
	}
	// Surprise
	for (int i = 1; i < 5; ++i)
	{
		FString RefText = FString::Printf(TEXT("/Script/Engine.Texture2D'/Game/PKH/Portaraits/Artist/Artist_Surprise%d.Artist_Surprise%d'"), i, i);
		static ConstructorHelpers::FObjectFinder<UTexture2D> Portrait_SurpriseRef(*RefText);
		if (Portrait_SurpriseRef.Object)
		{
			Portraits_Surprise.Add(Portrait_SurpriseRef.Object);
		}
	}
	// Sad
	for (int i = 1; i < 5; ++i)
	{
		FString RefText = FString::Printf(TEXT("/Script/Engine.Texture2D'/Game/PKH/Portaraits/Artist/Artist_Sad%d.Artist_Sad%d'"), i, i);
		static ConstructorHelpers::FObjectFinder<UTexture2D> Portrait_SadRef(*RefText);
		if (Portrait_SadRef.Object)
		{
			Portraits_Sadness.Add(Portrait_SadRef.Object);
		}
	}
	// Anger
	for (int i = 1; i < 5; ++i)
	{
		FString RefText = FString::Printf(TEXT("/Script/Engine.Texture2D'/Game/PKH/Portaraits/Artist/Artist_Anger%d.Artist_Anger%d'"), i, i);
		static ConstructorHelpers::FObjectFinder<UTexture2D> Portrait_AngerRef(*RefText);
		if (Portrait_AngerRef.Object)
		{
			Portraits_Anger.Add(Portrait_AngerRef.Object);
		}
	}
	// Default
	for (int i = 1; i < 5; ++i)
	{
		FString RefText = FString::Printf(TEXT("/Script/Engine.Texture2D'/Game/PKH/Portaraits/Artist/Artist_Default%d.Artist_Default%d'"), i, i);
		static ConstructorHelpers::FObjectFinder<UTexture2D> Portrait_DefaultRef(*RefText);
		if (Portrait_DefaultRef.Object)
		{
			Portraits_Default.Add(Portrait_DefaultRef.Object);
		}
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

	// Object
	Easel = GetWorld()->SpawnActor(EaselClass);
	Easel->SetActorEnableCollision(false);
	Easel->SetActorHiddenInGame(true);
}

void ANPC_Artist::DoJob()
{
	Super::DoJob();

	
}

void ANPC_Artist::StartSit()
{
	Super::StartSit();

	const FVector TargetLoc = GetActorLocation() + GetActorForwardVector() * -50.0f;
	SetActorLocation(TargetLoc);
}

void ANPC_Artist::EndSit()
{
	Super::EndSit();

	if(Easel->IsHidden())
	{
		const FVector TargetLoc = GetActorLocation() + GetActorForwardVector() * SitDistance;
		SetActorLocation(TargetLoc);

		Easel->SetActorLocationAndRotation(GetActorLocation() + EaselLoc, GetActorRotation() + EaselRot);
		Easel->SetActorEnableCollision(true);
		Easel->SetActorHiddenInGame(false);
	}
}

void ANPC_Artist::StandUp()
{
	Super::StandUp();

	Easel->SetActorEnableCollision(false);
	Easel->SetActorHiddenInGame(true);

	const FVector TargetLoc = GetActorLocation() - (GetActorForwardVector() * SitDistance);
	SetActorLocation(TargetLoc);
}

#pragma region override
void ANPC_Artist::OnConversationEnd()
{
	if (NPCController->GetIsWorking() && MyGameMode->GetHour() >= HOUR_BACK_HOME)
	{
		if(AnimInstance->GetCurrentActiveMontage() == Montage_Work)
		{
			AnimInstance->StopSpecificMontage(Montage_Work);
			AnimInstance->PlayMontage_Custom(Montage_StandUp);
		}
		NPCController->MoveToHome();

		Easel->SetActorEnableCollision(false);
		Easel->SetActorHiddenInGame(true);
	}
}

bool ANPC_Artist::CanRotateInWorking()
{
	return false;
}
#pragma endregion

void ANPC_Artist::OnHourUpdated(int32 NewHour)
{
	if(NewHour == HOUR_GO_HILL)
	{
		NPCController->MoveToTargetLoc(HillLoc);
		SetNPCWalk();
		return;
	}

	if (NewHour == HOUR_GO_PARK)
	{
		NPCController->MoveToTargetLoc(ParkLoc);
		NPCController->SetIsWorking(true);
		SetNPCWalk();
		return;
	}

	if(NewHour == HOUR_BACK_HOME)
	{
		if(false == NPCController->IsInConversation())
		{
			AnimInstance->StopSpecificMontage(Montage_Work);
			AnimInstance->PlayMontage_Custom(Montage_StandUp);
			SetNPCWalk();
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
	NPCController->MoveToHome();
}
