// Fill out your copyright notice in the Description page of Project Settings.


#include "LSH/NPC/NPC_Farmer1.h"

#include "PKH/Animation/NPCAnimInstance.h"
#include "PKH/NPC/NPCController.h"
#include "Kismet/GameplayStatics.h"

#define HOUR_WORK 9
#define HOUR_GO_CAFE 14
#define HOUR_WORK2 20
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

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("farming"), FoundActors);
	for (AActor* Actor : FoundActors)
	{
		if (Actor == nullptr)break;
		UE_LOG(LogTemp, Warning, TEXT("Found actor: %s"), *Actor->GetName());
		FarmLoc = Actor->GetActorLocation();
		WorkRotation = Actor->GetActorRotation();
	}
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("cafeSitting"), FoundActors);
	for (AActor* Actor : FoundActors)
	{
		if (Actor == nullptr)break;
		UE_LOG(LogTemp, Warning, TEXT("Found actor: %s"), *Actor->GetName());
		CafeLoc = Actor->GetActorLocation();
		CafeRot = Actor->GetActorRotation();
	}
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
		WorkRotation = FarmRot;
		NPCController->MoveToTargetLoc(FarmLoc);
		NPCController->SetIsWorking(true);
		return;
	}

	if (NewHour == HOUR_GO_CAFE)
	{
		AnimInstance->StopSpecificMontage(Montage_Work);
		Montage_Work = Montage_Sit;
		WorkRotation = CafeRot;
		NPCController->MoveToTargetLoc(CafeLoc);
		NPCController->SetIsWorking(true);
		return;
	}

	if (NewHour == HOUR_WORK2)
	{
		AnimInstance->StopSpecificMontage(Montage_Work);
		Montage_Work = Montage_FarmWork;
		WorkRotation = FarmRot;
		NPCController->MoveToTargetLoc(FarmLoc);
		NPCController->SetIsWorking(true);
	}

	if (NewHour == HOUR_HOME)
	{
		NPCController->MoveToTargetLoc(FarmLoc);
		NPCController->SetIsWorking(false);
		AnimInstance->StopSpecificMontage(Montage_Work);
	}
}