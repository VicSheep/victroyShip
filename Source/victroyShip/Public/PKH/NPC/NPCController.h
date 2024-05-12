// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "NPCController.generated.h"

/**
 * 
 */
UCLASS()
class VICTROYSHIP_API ANPCController : public AAIController
{
	GENERATED_BODY()
	
public:
	ANPCController();

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UBlackboardData> BBData;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UBehaviorTree> BTData;

	virtual void OnPossess(APawn* InPawn) override;

public:
	void RunAI();
	void StopAI();

};
