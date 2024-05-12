// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/BT/BTTask_SetTargetLoc.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_SetTargetLoc::UBTTask_SetTargetLoc()
{

}

EBTNodeResult::Type UBTTask_SetTargetLoc::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	APawn* OwnerPawn = OwnerComp.GetAIOwner()->GetPawn();
	if(nullptr == OwnerPawn)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	if(nullptr == BBComp)
	{
		return EBTNodeResult::Failed;
	}

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	if (nullptr == NavSystem)
	{
		return EBTNodeResult::Failed;
	}

	const FVector Origin = OwnerPawn->GetActorLocation();
	float PatrolRadius = BBComp->GetValueAsFloat(TEXT("PatrolDistance"));
	FNavLocation NextNav;
	if (NavSystem->GetRandomPointInNavigableRadius(Origin, PatrolRadius, NextNav))
	{
		BBComp->SetValueAsVector(TEXT("TargetLoc"), NextNav.Location);
		return EBTNodeResult::Succeeded;
	}

	return Result;
}
