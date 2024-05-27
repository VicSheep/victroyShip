// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/BT/BTTask_DoJob.h"

#include "AIController.h"
#include "PKH/NPC/NPCBase.h"

UBTTask_DoJob::UBTTask_DoJob()
{

}

EBTNodeResult::Type UBTTask_DoJob::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type SuperReuslt = Super::ExecuteTask(OwnerComp, NodeMemory);

	APawn* OwnerPawn = OwnerComp.GetAIOwner()->GetPawn();
	if(nullptr == OwnerPawn)
	{
		return EBTNodeResult::Failed;
	}

	ANPCBase* NPC = Cast<ANPCBase>(OwnerPawn);
	if(nullptr == NPC)
	{
		return EBTNodeResult::Failed;
	}

	NPC->DoJob();
	return EBTNodeResult::Succeeded;
}
