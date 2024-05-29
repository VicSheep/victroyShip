// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_IsFriendly.generated.h"

/**
 * 
 */
UCLASS()
class VICTROYSHIP_API UBTDecorator_IsFriendly : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_IsFriendly();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

};
