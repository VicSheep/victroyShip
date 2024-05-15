// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/NPC/NPCController.h"

#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

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
	BBComp->SetValueAsBool(TEXT("InConversation"), true);
}

void ANPCController::EndConversation()
{
	BBComp->SetValueAsBool(TEXT("InConversation"), false);
}

void ANPCController::MoveToTargetLoc()
{
	BBComp->SetValueAsBool(TEXT("IsMoving"), true);
}

void ANPCController::ReachTargetLoc()
{
	BBComp->SetValueAsBool(TEXT("IsMoving"), false);
}
