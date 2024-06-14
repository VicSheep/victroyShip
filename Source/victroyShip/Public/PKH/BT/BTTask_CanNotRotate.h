// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_CanNotRotate.generated.h"

/**
 * 
 */
UCLASS()
class VICTROYSHIP_API UBTTask_CanNotRotate : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_CanNotRotate();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};