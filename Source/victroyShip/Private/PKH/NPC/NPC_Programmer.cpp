// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/NPC/NPC_Programmer.h"

#include "PKH/Animation/NPCAnimInstance.h"
#include "PKH/NPC/NPCController.h"

#define HOUR_GO_WORK 9
#define HOUR_GO_PARK 15
#define HOUR_BACK_HOME 17

ANPC_Programmer::ANPC_Programmer()
{
	NPCType = ENPCType::Programmer;

	HomeLoc = FVector(-4372, 4411, 983);
	WorkLoc = FVector(1480, 3340, 631); 
	ParkLoc = FVector(-640, 2680, 520);

	WorkRotation = FRotator(0, 200, 0);

	SitDistance = -50.0f;

	PreferItemName = TEXT("Cauliflower,EggPlant");

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

	// Object
	static ConstructorHelpers::FClassFinder<AActor> LaptopClassRef(TEXT("/Game/PKH/Blueprint/BP_Laptop.BP_Laptop_C"));
	if (LaptopClassRef.Class)
	{
		LaptopClass = LaptopClassRef.Class;
	}

	// Portraits
	// Joy
	for (int i = 1; i < 5; ++i)
	{
		FString RefText = FString::Printf(TEXT("/Script/Engine.Texture2D'/Game/PKH/Portaraits/Programmer/Programmer_Joy%d.Programmer_Joy%d'"), i, i);
		static ConstructorHelpers::FObjectFinder<UTexture2D> Portrait_JoyRef(*RefText);
		if (Portrait_JoyRef.Object)
		{
			Portraits_Joy.Add(Portrait_JoyRef.Object);
		}
	}
	// Surprise
	for (int i = 1; i < 5; ++i)
	{
		FString RefText = FString::Printf(TEXT("/Script/Engine.Texture2D'/Game/PKH/Portaraits/Programmer/Programmer_Surprise%d.Programmer_Surprise%d'"), i, i);
		static ConstructorHelpers::FObjectFinder<UTexture2D> Portrait_SurpriseRef(*RefText);
		if (Portrait_SurpriseRef.Object)
		{
			Portraits_Surprise.Add(Portrait_SurpriseRef.Object);
		}
	}
	// Sad
	for (int i = 1; i < 5; ++i)
	{
		FString RefText = FString::Printf(TEXT("/Script/Engine.Texture2D'/Game/PKH/Portaraits/Programmer/Programmer_Sad%d.Programmer_Sad%d'"), i, i);
		static ConstructorHelpers::FObjectFinder<UTexture2D> Portrait_SadRef(*RefText);
		if (Portrait_SadRef.Object)
		{
			Portraits_Sadness.Add(Portrait_SadRef.Object);
		}
	}
	// Anger
	for (int i = 1; i < 5; ++i)
	{
		FString RefText = FString::Printf(TEXT("/Script/Engine.Texture2D'/Game/PKH/Portaraits/Programmer/Programmer_Anger%d.Programmer_Anger%d'"), i, i);
		static ConstructorHelpers::FObjectFinder<UTexture2D> Portrait_AngerRef(*RefText);
		if (Portrait_AngerRef.Object)
		{
			Portraits_Anger.Add(Portrait_AngerRef.Object);
		}
	}
	// Default
	for (int i = 1; i < 5; ++i)
	{
		FString RefText = FString::Printf(TEXT("/Script/Engine.Texture2D'/Game/PKH/Portaraits/Programmer/Programmer_Default%d.Programmer_Default%d'"), i, i);
		static ConstructorHelpers::FObjectFinder<UTexture2D> Portrait_DefaultRef(*RefText);
		if (Portrait_DefaultRef.Object)
		{
			Portraits_Default.Add(Portrait_DefaultRef.Object);
		}
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

	// Object
	Laptop = GetWorld()->SpawnActor(LaptopClass);
	Laptop->SetActorEnableCollision(false);
	Laptop->SetActorHiddenInGame(true);
}

void ANPC_Programmer::DoJob()
{
	SetActorRotation(WorkRotation);
	if (AnimInstance->GetCurrentActiveMontage() != Montage_Work && AnimInstance->GetCurrentActiveMontage() != Montage_StandUp)
	{
		AnimInstance->PlayMontage_Custom(Montage_Work);
	}
}

void ANPC_Programmer::StartSit()
{
	Super::StartSit();

	const FVector TargetLoc = GetActorLocation() + GetActorForwardVector() * -60.0f;
	SetActorLocation(TargetLoc);
}

#pragma region override
void ANPC_Programmer::EndSit()
{
	Super::EndSit();

	if(Laptop->IsHidden())
	{
		const FVector TargetLoc = GetActorLocation() + GetActorForwardVector() * SitDistance;
		SetActorLocation(TargetLoc);

		Laptop->SetActorLocationAndRotation(GetActorLocation() + LaptopLoc, GetActorRotation() + LaptopRot);
		Laptop->SetActorEnableCollision(true);
		Laptop->SetActorHiddenInGame(false);
	}
}

void ANPC_Programmer::StandUp()
{
	Super::StandUp();

	Laptop->SetActorEnableCollision(false);
	Laptop->SetActorHiddenInGame(true);

	const FVector TargetLoc = GetActorLocation() - (GetActorForwardVector() * SitDistance);
	SetActorLocation(TargetLoc);
}

void ANPC_Programmer::OnConversationEnd()
{
	if(NPCController->GetIsWorking() && MyGameMode->GetHour() >= HOUR_GO_PARK)
	{
		AnimInstance->StopSpecificMontage(Montage_Work);
		AnimInstance->PlayMontage_Custom(Montage_StandUp);
	}
}

bool ANPC_Programmer::CanRotateInWorking()
{
	return false;
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
