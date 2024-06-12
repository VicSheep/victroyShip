// Fill out your copyright notice in the Description page of Project Settings.


#include "LSH/NPC/NPC_Neet.h"

#include "PKH/Animation/NPCAnimInstance.h"
#include "PKH/NPC/NPCController.h"
#include "Kismet/GameplayStatics.h"

#define HOUR_SIT 9
#define HOUR_SLEEP 11
#define HOUR_Dance 16

ANPC_Neet::ANPC_Neet()
{
	NPCType = ENPCType::Unemployed;//юс╫ц

	HomeLoc = FVector(1450, -2180, 88);

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

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("neetSleeping"), FoundActors);
	for (AActor* Actor : FoundActors)
	{
		if (Actor == nullptr)break;
		UE_LOG(LogTemp, Warning, TEXT("Found actor: %s"), *Actor->GetName());
		NeetSleepingLoc = Actor->GetActorLocation();
		NeetSleepingRot = Actor->GetActorRotation();
	}
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("neetSitting"), FoundActors);
	for (AActor* Actor : FoundActors)
	{
		if (Actor == nullptr)break;
		UE_LOG(LogTemp, Warning, TEXT("Found actor: %s"), *Actor->GetName());
		NeetSittingLoc = Actor->GetActorLocation();
		NeetSittingRot = Actor->GetActorRotation();
	}
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("neetDancing"), FoundActors);
	for (AActor* Actor : FoundActors)
	{
		if (Actor == nullptr)break;
		UE_LOG(LogTemp, Warning, TEXT("Found actor: %s"), *Actor->GetName());
		NeetDancingLoc = Actor->GetActorLocation();
		NeetDancingRot = Actor->GetActorRotation();
	}
}

void ANPC_Neet::DoJob()
{
	Super::DoJob();


}

void ANPC_Neet::OnHourUpdated(int32 NewHour)
{
	if (NewHour == HOUR_SIT)
	{
		Montage_Work = Montage_Sit;
		NPCController->MoveToTargetLoc(NeetSittingLoc);
		WorkRotation = NeetSittingRot;
		NPCController->SetIsWorking(true);
		return;
	}

	if (NewHour == HOUR_SLEEP)
	{
		AnimInstance->StopSpecificMontage(Montage_Work);
		Montage_Work = Montage_Sleep;
		WorkRotation = NeetSleepingRot;
		NPCController->MoveToTargetLoc(NeetSleepingLoc);
		NPCController->SetIsWorking(true);
		return;
	}

	if (NewHour == HOUR_Dance)
	{
		AnimInstance->StopSpecificMontage(Montage_Work);
		Montage_Work = Montage_Dance;
		NPCController->MoveToTargetLoc(NeetDancingLoc);
		WorkRotation = NeetDancingRot;
		NPCController->SetIsWorking(true);
	}
}