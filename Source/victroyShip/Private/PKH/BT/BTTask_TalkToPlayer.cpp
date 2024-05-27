// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/BT/BTTask_TalkToPlayer.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "PKH/NPC/NPCBase.h"

UBTTask_TalkToPlayer::UBTTask_TalkToPlayer()
{
	NodeName = TEXT("Talk2Player");
}

EBTNodeResult::Type UBTTask_TalkToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type SuperResult =  Super::ExecuteTask(OwnerComp, NodeMemory);

	APawn* OwnerPawn = OwnerComp.GetAIOwner()->GetPawn();
	if(nullptr == OwnerPawn)
	{
		return EBTNodeResult::Failed;
	}

	ANPCBase* NPC = Cast <ANPCBase>(OwnerPawn);
	if(nullptr == NPC)
	{
		return EBTNodeResult::Failed;
	}

	NPC->GreetingToPlayer();
	return EBTNodeResult::Succeeded;
}
