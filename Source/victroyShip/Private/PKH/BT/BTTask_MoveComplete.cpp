// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/BT/BTTask_MoveComplete.h"

UBTTask_MoveComplete::UBTTask_MoveComplete()
{
	NodeName = TEXT("MoveDone");
}

EBTNodeResult::Type UBTTask_MoveComplete::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);


	return Result;
}
