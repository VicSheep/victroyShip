// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/BT/BTDecorator_CanRotate.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "PKH/BT/BTNPCKey.h"
#include "PKH/NPC/NPCBase.h"

UBTDecorator_CanRotate::UBTDecorator_CanRotate()
{
}

bool UBTDecorator_CanRotate::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool SuperResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	if (nullptr == BBComp)
	{
		return false;
	}

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

	if(BBComp->GetValueAsBool(KEY_IS_WORKING) && false == NPC->CanRotateInWorking())
	{
		return false;
	}

	return true;
}
