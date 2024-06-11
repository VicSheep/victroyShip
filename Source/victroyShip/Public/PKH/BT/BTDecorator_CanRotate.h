// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_CanRotate.generated.h"

/**
 * 
 */
UCLASS()
class VICTROYSHIP_API UBTDecorator_CanRotate : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	UBTDecorator_CanRotate();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
