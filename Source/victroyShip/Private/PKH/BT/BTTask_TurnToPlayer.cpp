// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/BT/BTTask_TurnToPlayer.h"

#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "PKH/NPC/NPCBase.h"

UBTTask_TurnToPlayer::UBTTask_TurnToPlayer()
{
	NodeName = TEXT("Turn");
}

EBTNodeResult::Type UBTTask_TurnToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type SuperResult = Super::ExecuteTask(OwnerComp, NodeMemory);

	ANPCBase* NPC = Cast<ANPCBase>(OwnerComp.GetAIOwner()->GetPawn());
	if(nullptr == NPC)
	{
		return EBTNodeResult::Failed;
	}

	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if(nullptr == Player)
	{
		return EBTNodeResult::Failed;
	}

	FVector DirVec = (Player->GetActorLocation() - NPC->GetActorLocation()).GetSafeNormal();
	DirVec.Z = 0;
	const FRotator TargetRot = DirVec.ToOrientationRotator();
	NPC->SetActorRotation(TargetRot);

	return EBTNodeResult::Succeeded;
}
