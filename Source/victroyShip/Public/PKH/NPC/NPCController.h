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

protected:
	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION()
	void OnSightUpdated(AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION()
	void OnLostPlayer();

public:
	// Run / Stop AI
	void RunAI();
	void StopAI();

	// Conversation
	void StartConversation();
	void EndConversation();

	// Move
	void SetHomeLoc(const FVector& HomeLoc);
	FVector GetHomeLoc() const;

	void MoveToTargetLoc(const FVector& TargetLoc);
	void MoveToHome();

	// Is NPC In Conversation?
	bool IsInConversation();

	bool IsMovingSomewhere();

	// For Reset Keys
	void ResetBBKeys();

	// Stop Near Player
	void PlayerInRange(AActor* Player);
	void PlayerOutRange();

// Do Job
protected:
	bool IsWorking = false;

public:
	void SetIsWorking(bool InIsWorking);
	FORCEINLINE bool GetIsWorking() const { return IsWorking; }

	bool IsWorkInNow();

};
