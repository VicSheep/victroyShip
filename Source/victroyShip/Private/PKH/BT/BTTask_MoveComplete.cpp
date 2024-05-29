// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/BT/BTTask_MoveComplete.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "PKH/BT/BTNPCKey.h"
#include "PKH/NPC/NPCController.h"

UBTTask_MoveComplete::UBTTask_MoveComplete()
{
	NodeName = TEXT("MoveDone");
}

EBTNodeResult::Type UBTTask_MoveComplete::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	ANPCController* NPCController = Cast<ANPCController>(OwnerComp.GetAIOwner());
	if(nullptr == NPCController)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	if (BBComp)
	{
		BBComp->SetValueAsBool(KEY_IS_MOVING, false);
		if(NPCController->GetIsWorking())
		{
			BBComp->SetValueAsBool(KEY_IS_WORKING, true);
		}
		return EBTNodeResult::Succeeded;
	}

	return Result;
}
