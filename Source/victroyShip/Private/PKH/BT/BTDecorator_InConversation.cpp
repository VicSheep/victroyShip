// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/BT/BTDecorator_InConversation.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "PKH/BT/BTNPCKey.h"

UBTDecorator_InConversation::UBTDecorator_InConversation()
{

}

bool UBTDecorator_InConversation::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool SuperResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	if(BBComp)
	{
		return BBComp->GetValueAsBool(KEY_IN_CONV);
	}

	return SuperResult;
}
