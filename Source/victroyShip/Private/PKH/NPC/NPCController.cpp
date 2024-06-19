// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/NPC/NPCController.h"

#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "OJS/Player/FarmLifeOjsPlayerCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "PKH/BT/BTNPCKey.h"
#include "PKH/NPC/NPCBase.h"

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
	if(false == Actor->IsA<AFarmLifeOjsPlayerCharacter>())
	{
		return;
	}

	ANPCBase* NPC = Cast<ANPCBase>(GetPawn());
	if (nullptr == NPC || false == NPC->IsFriendly())
	{
		return;
	}
	
	AFarmLifeGameMode* MyGameMode = Cast<AFarmLifeGameMode>(GetWorld()->GetAuthGameMode());
	if(MyGameMode->IsInConversation())
	{
		return;
	}

	if(BBComp->GetValueAsBool(KEY_IS_WORKING))
	{
		return;
	}

	if(Stimulus.WasSuccessfullySensed())
	{
		NPC->SetNPCRun();
		if(false == BBComp->GetValueAsBool(KEY_PLAYER_IN_SIGHT))
		{
			NPC->SetCurEmotion(EEmotion::noticed);
			NPC->PlayEmotion(true);
		}

		BBComp->SetValueAsObject(KEY_PLAYER, Actor);
		BBComp->SetValueAsBool(KEY_PLAYER_IN_SIGHT, true);
		
		if(GetWorldTimerManager().IsTimerActive(SightHandle))
		{
			GetWorldTimerManager().ClearTimer(SightHandle);
		}
	}
	else
	{
		GetWorldTimerManager().SetTimer(SightHandle, this, &ANPCController::OnLostPlayer, TIME_LIMIT, false);
	}
}

void ANPCController::OnLostPlayer()
{
	// �̹� ����ƴٸ� ��ŵ
	if(nullptr == BBComp->GetValueAsObject(KEY_PLAYER))
	{
		return;
	}

	ANPCBase* NPC = Cast<ANPCBase>(GetPawn());
	if(NPC)
	{
		if(BBComp->GetValueAsBool(KEY_IS_MOVING))
		{
			NPC->SetNPCWalk();
		}
		else
		{
			NPC->SetNPCPatrol();
		}
		
		NPC->SetEmotionUI(false);
	}

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

		// �ʱ�ȭ

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
	BBComp->SetValueAsBool(KEY_IN_CONV, true);

	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	BBComp->SetValueAsObject(KEY_PLAYER, Player);
}

void ANPCController::EndConversation()
{
	BBComp->SetValueAsBool(KEY_IN_CONV, false);
	BBComp->SetValueAsObject(KEY_PLAYER, nullptr);
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
	
	ANPCBase* NPC = Cast<ANPCBase>(GetPawn());
	if (NPC)
	{
		NPC->SetNPCWalk();
	}
}

void ANPCController::MoveToHome()
{
	BBComp->SetValueAsBool(KEY_IS_MOVING, true);
	const FVector& HomeLoc = BBComp->GetValueAsVector(KEY_HOME_LOC);
	BBComp->SetValueAsVector(KEY_TARGET_LOC, HomeLoc);

	ANPCBase* NPC = Cast<ANPCBase>(GetPawn());
	if (NPC)
	{
		NPC->SetNPCWalk();
	}
}

bool ANPCController::IsInConversation()
{
	return BBComp->GetValueAsBool(KEY_IN_CONV);
}

bool ANPCController::IsMovingSomewhere()
{
	return BBComp->GetValueAsBool(KEY_IS_MOVING);
}

void ANPCController::ResetBBKeys()
{
	BBComp->SetValueAsBool(KEY_IN_CONV, false);
	BBComp->SetValueAsBool(KEY_IS_WORKING, false);
	BBComp->SetValueAsBool(KEY_IS_MOVING, false);
	BBComp->SetValueAsBool(KEY_PLAYER_IN_SIGHT, false);

	BBComp->SetValueAsObject(KEY_PLAYER, nullptr);
}

FVector ANPCController::GetHomeLoc() const
{
	return BBComp->GetValueAsVector(KEY_HOME_LOC);
}
#pragma endregion


void ANPCController::SetIsWorking(bool InIsWorking)
{
	IsWorking = InIsWorking;
	// �� ������ ��쿡�� �ٷ� Ű���� ����
	if(false == IsWorking)
	{
		BBComp->SetValueAsBool(KEY_IS_WORKING, false);
	}
}
