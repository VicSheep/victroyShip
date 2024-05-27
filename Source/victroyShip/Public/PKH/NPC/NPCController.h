// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
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

	UPROPERTY(EditDefaultsOnly)
	class UBlackboardComponent* BBComp;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UAIPerceptionComponent> SightComp;

	FTimerHandle SightHandle;

	float NormalWalkSpeed = 150.0f;
	float FastWalkSpeed = 400.0f;

protected:
	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION()
	void OnSightUpdated(AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION()
	void OnLostPlayer();

public:
	void RunAI();
	void StopAI();

	void StartConversation();
	void EndConversation();

	void SetHomeLoc(const FVector& HomeLoc);
	FVector GetHomeLoc() const;

	void MoveToTargetLoc(const FVector& TargetLoc);
	void MoveToHome();

	

};
