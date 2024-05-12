// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/BT/BTDecorator_MoveToTarget.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_MoveToTarget::UBTDecorator_MoveToTarget()
{

}

bool UBTDecorator_MoveToTarget::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool SuperResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	if(BBComp)
	{
		return BBComp->GetValueAsBool(TEXT("InConversation"));
	}

	return SuperResult;
}
