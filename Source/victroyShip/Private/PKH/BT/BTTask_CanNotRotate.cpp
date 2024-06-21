// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/BT/BTTask_CanNotRotate.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "PKH/BT/BTNPCKey.h"
#include "PKH/NPC/NPCBase.h"

UBTTask_CanNotRotate::UBTTask_CanNotRotate()
{

}

EBTNodeResult::Type UBTTask_CanNotRotate::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type SuperResult = Super::ExecuteTask(OwnerComp, NodeMemory);

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

	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	if(nullptr == BBComp)
	{
		return EBTNodeResult::Failed;
	}

	const bool CanNotRotateToPlayer = (false == NPC->CanRotateInWorking()) && BBComp->GetValueAsBool(KEY_IS_WORKING);
	return CanNotRotateToPlayer ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
