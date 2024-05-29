// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/NPC/NPCController.h"

#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "PKH/BT/BTNPCKey.h"
#include "PKH/NPC/NPCBase.h"
#include "PKH/Test/STTCharacter.h"

#define TIME_LIMIT 3.0f

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

	SightComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("SightComp"));
}

void ANPCController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	SightComp->OnTargetPerceptionUpdated.AddDynamic(this, &ANPCController::OnSightUpdated);

	RunAI();
}


#pragma region AI Perception 
void ANPCController::OnSightUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if(false == Actor->IsA<ASTTCharacter>()) // 추후 수정할 것
	{
		return;
	}

	ANPCBase* NPC = Cast<ANPCBase>(GetPawn());
	if (nullptr == NPC || false == NPC->IsFriendly())
	{
		return;
	}

	if(BBComp->GetValueAsBool(KEY_IS_WORKING))
	{
		return;
	}

	if(Stimulus.WasSuccessfullySensed())
	{
		NPC->GetCharacterMovement()->MaxWalkSpeed = FastWalkSpeed;

		BBComp->SetValueAsObject(KEY_PLAYER, Actor);
		BBComp->SetValueAsBool(KEY_PLAYER_IN_SIGHT, true);
		
		GetWorldTimerManager().ClearTimer(SightHandle);
	}
	else
	{
		GetWorldTimerManager().SetTimer(SightHandle, this, &ANPCController::OnLostPlayer, TIME_LIMIT, false);
	}
}

void ANPCController::OnLostPlayer()
{
	// 이미 실행됐다면 스킵
	if(nullptr == BBComp->GetValueAsObject(KEY_PLAYER))
	{
		return;
	}

	ACharacter* NPC = CastChecked<ACharacter>(GetPawn());
	NPC->GetCharacterMovement()->MaxWalkSpeed = NormalWalkSpeed;

	BBComp->SetValueAsObject(KEY_PLAYER, nullptr);
	BBComp->SetValueAsBool(KEY_PLAYER_IN_SIGHT, false);
}
#pragma endregion

#pragma region Run / Stop AI
void ANPCController::RunAI()
{
	BBComp = Blackboard;
	if(UseBlackboard(BBData, BBComp))
	{
		bool result = RunBehaviorTree(BTData);

		// 초기화

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
#pragma endregion

#pragma region Conversation
void ANPCController::StartConversation()
{
	BBComp->SetValueAsBool(KEY_IN_CONV, true); UE_LOG(LogTemp, Log, TEXT("StartConversation() In Controller"));
}

void ANPCController::EndConversation()
{
	BBComp->SetValueAsBool(KEY_IN_CONV, false);
	OnLostPlayer();
}
#pragma endregion

#pragma region Location
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
#pragma endregion


void ANPCController::SetIsWorking(bool InIsWorking)
{
	IsWorking = InIsWorking;
	// 일 종료인 경우에는 바로 키까지 변경
	if(false == IsWorking)
	{
		BBComp->SetValueAsBool(KEY_IS_WORKING, false);
	}
}
