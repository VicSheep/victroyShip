// Fill out your copyright notice in the Description page of Project Settings.


#include "LSH/NPC/NPC_Fisher.h"

#include "PKH/Animation/NPCAnimInstance.h"
#include "PKH/NPC/NPCController.h"
#include "Kismet/GameplayStatics.h"

#define HOUR_FISHING 9
#define HOUR_BACK_HOME 16

ANPC_Fisher::ANPC_Fisher()
{
	NPCType = ENPCType::Fisherman;//юс╫ц

	HomeLoc = FVector(1450, -2180, 88);
	FishLoc = FVector(-980, -1410, 88);

	WorkRotation = FRotator(0, 270, 0);

	static ConstructorHelpers::FObjectFinder<UAnimMontage> Montage_WorkRef(TEXT("/Script/Engine.AnimMontage'/Game/PKH/Anim/AM_ArtistWork.AM_ArtistWork'"));
	if (Montage_WorkRef.Object)
	{
		Montage_Work = Montage_WorkRef.Object;
	}
}

void ANPC_Fisher::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("fishing"), FoundActors);
	for (AActor* Actor : FoundActors)
	{
		if (Actor == nullptr)break;
		UE_LOG(LogTemp, Warning, TEXT("Found actor: %s"), *Actor->GetName());
		FishLoc = Actor->GetActorLocation();
		FishRot = Actor->GetActorRotation();
	}
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("FisherHome"), FoundActors);
	for (AActor* Actor : FoundActors)
	{
		if (Actor == nullptr)break;
		UE_LOG(LogTemp, Warning, TEXT("Found actor: %s"), *Actor->GetName());
		FisherHomeLoc = Actor->GetActorLocation();
		FisherHomeRot = Actor->GetActorRotation();
	}
}

void ANPC_Fisher::DoJob()
{
	Super::DoJob();


}

void ANPC_Fisher::OnHourUpdated(int32 NewHour)
{
	if (NewHour == HOUR_FISHING)
	{
		Montage_Work = Montage_Fishing;
		WorkRotation = FishRot;
		NPCController->MoveToTargetLoc(FishLoc);
		NPCController->SetIsWorking(true);
		return;
	}

	if (NewHour == HOUR_BACK_HOME)
	{
		AnimInstance->StopSpecificMontage(Montage_Work);
		Montage_Work = Montage_Drink;
		NPCController->MoveToTargetLoc(FisherHomeLoc);
		WorkRotation = FisherHomeRot;
		NPCController->SetIsWorking(true);
		return;
	}

}