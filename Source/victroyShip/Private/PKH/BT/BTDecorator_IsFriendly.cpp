// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/BT/BTDecorator_IsFriendly.h"

#include "AIController.h"
#include "PKH/NPC/NPCBase.h"

UBTDecorator_IsFriendly::UBTDecorator_IsFriendly()
{

}

bool UBTDecorator_IsFriendly::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool SuperResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	APawn* OwnerPawn = OwnerComp.GetAIOwner()->GetPawn();
	if(nullptr == OwnerPawn)
	{
		return false;
	}

	ANPCBase* NPC = Cast<ANPCBase>(OwnerPawn);
	if(nullptr == NPC)
	{
		return false;
	}

	return NPC->IsFriendly();
}
