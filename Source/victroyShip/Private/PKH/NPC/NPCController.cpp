// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/NPC/NPCController.h"

#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "PKH/BT/BTNPCKey.h"

ANPCController::ANPCController()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBDataRef(TEXT("/Script/AIModule.BlackboardData'/Game/PKH/BT/BB_NPC.BB_NPC'"));
	if (BBDataRef.Object)
	{
		BBData = BBDataRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTDataRef(TEXT("/Script/AIModule.BehaviorTree'/Game/PKH/BT/BT_NPC.BT_NPC'"));
	if (BTDataRef.Object)
	{
		BTData = BTDataRef.Object;
	}
}

void ANPCController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	RunAI();
}

void ANPCController::RunAI()
{
	BBComp = Blackboard;
	if(UseBlackboard(BBData, BBComp))
	{
		bool result = RunBehaviorTree(BTData);

		// √ ±‚»≠

	}
}

void ANPCController::StopAI()
{
	UBehaviorTreeComponent* BTComp = Cast<UBehaviorTreeComponent>(BrainComponent);
	if(BTComp)
	{
		BTComp->StopTree();
	}
}

void ANPCController::StartConversation()
{
	BBComp->SetValueAsBool(KEY_IN_CONV, true);
}

void ANPCController::EndConversation()
{
	BBComp->SetValueAsBool(KEY_IN_CONV, false);
}

void ANPCController::SetHomeLoc(const FVector& HomeLoc)
{
	BBComp->SetValueAsVector(KEY_HOME_LOC, HomeLoc);
}

void ANPCController::MoveToTargetLoc(const FVector& TargetLoc)
{
	BBComp->SetValueAsBool(KEY_IS_MOVING, true);
	BBComp->SetValueAsVector(KEY_TARGET_LOC, TargetLoc);
}

void ANPCController::MoveToHome()
{
	BBComp->SetValueAsBool(KEY_IS_MOVING, true);
	const FVector& HomeLoc = BBComp->GetValueAsVector(KEY_HOME_LOC);
	BBComp->SetValueAsVector(KEY_TARGET_LOC, HomeLoc);
}

FVector ANPCController::GetHomeLoc() const
{
	return BBComp->GetValueAsVector(KEY_HOME_LOC);
}
