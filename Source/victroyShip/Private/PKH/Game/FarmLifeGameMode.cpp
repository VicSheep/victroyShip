﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/Game/FarmLifeGameMode.h"

#include "Engine/DirectionalLight.h"
#include "Kismet/GameplayStatics.h"
#include "PKH/Http/HttpActor.h"
#include "PKH/Interface/DateUpdate.h"
#include "PKH/Interface/HourUpdate.h"
#include "PKH/NPC/NPCBase.h"
#include "PKH/UI/NPCConversationWidget.h"
#include "PKH/UI/TimerWidget.h"
#include "Serialization/EditorBulkData.h"

#define TEN_MINUTES 10
#define SIXTY_MINUTES 60
#define START_HOUR 8
#define END_HOUR 18
#define TIME_UPDATE_INTERVAL 5.0f

AFarmLifeGameMode::AFarmLifeGameMode()
{
	// Default Pawn & Controller


	// Http Actor
	static ConstructorHelpers::FClassFinder<AHttpActor> HttpActorClassRef(TEXT("/Game/PKH/Blueprint/BP_HttpActor.BP_HttpActor_C"));
	if(HttpActorClassRef.Class)
	{
		HttpActorClass = HttpActorClassRef.Class;
	}

	// UI
	static ConstructorHelpers::FClassFinder<UNPCConversationWidget> ConversationUIClassRef(TEXT("/Game/PKH/UI/WBP_NPCConversation.WBP_NPCConversation_C"));
	if (ConversationUIClassRef.Class)
	{
		ConversationUIClass = ConversationUIClassRef.Class;
	}
	static ConstructorHelpers::FClassFinder<UTimerWidget> TimerUIClassRef(TEXT("/Game/PKH/UI/WBP_Timer.WBP_Timer_C"));
	if (TimerUIClassRef.Class)
	{
		TimerUIClass = TimerUIClassRef.Class;
	}
}

void AFarmLifeGameMode::BeginPlay()
{
	Super::BeginPlay();

	if(HttpActorClass)
	{
		HttpActor = GetWorld()->SpawnActor<AHttpActor>(HttpActorClass);
	}

	// Time flow
	SunLight = Cast<ADirectionalLight>(UGameplayStatics::GetActorOfClass(GetWorld(), ADirectionalLight::StaticClass()));
	SunLight->SetActorRotation(SunBeginRot);

	StartTime();

	// UI
	TimerUI = CreateWidget<UTimerWidget>(GetWorld(), TimerUIClass);
	ensure(TimerUI);
	TimerUI->AddToViewport();
	TimerUI->UpdateTimerUI(Date, Hours, Minutes);

	ConversationUI = CreateWidget<UNPCConversationWidget>(GetWorld(), ConversationUIClass);
	ensure(ConversationUI);
	ConversationUI->AddToViewport();
	ConversationUI->SetVisibility(ESlateVisibility::Hidden);

	// Initialize
	Initialize();
}

// 모델 로딩을 위해 최초 1회 통신
void AFarmLifeGameMode::Initialize()
{
	HttpActor->SendSpeech(InitialName, InitialPath, InitialNPC);
}

#pragma region NPC conversation
void AFarmLifeGameMode::SendSpeech(const FString& FileName, const FString& FilePath, const TObjectPtr<ANPCBase>& NewNPC)
{
	CurNPC = NewNPC;
	CurNPC->StartConversation();

	HttpActor->SendSpeech(FileName, FilePath, CurNPC->GetNPCName());
	ConversationUI->UpdateConversationUI(CurNPC->GetNPCName(), TEXT(""));
	ConversationUI->SetVisibility(ESlateVisibility::Visible);
}

void AFarmLifeGameMode::SetLatestSpeech(const FString& Response, const FString& FilePath)
{
	// 초기화용 호출이라면 바로 종료
	if(nullptr == CurNPC)
	{
		UE_LOG(LogTemp, Log, TEXT("Initialize Complete"));
		return;
	}

	LatestSpeech = Response;
	UE_LOG(LogTemp, Warning, TEXT("ReqText Complete : [%s] %s"), *CurNPC->GetNPCName(), *LatestSpeech);

	// UI 갱신
	if(ConversationUI->IsVisible())
	{
		ConversationUI->UpdateConversationUI(CurNPC->GetNPCName(), LatestSpeech);
		CurNPC->LoadSpeechFileAndPlay(FilePath);
	}

	// 호감도 갱신

}

FString& AFarmLifeGameMode::GetLatestSpeech()
{
	return LatestSpeech;
}

void AFarmLifeGameMode::EndConversation()
{
	if(CurNPC)
	{
		CurNPC->EndConversation();
	}
}

// By Text
void AFarmLifeGameMode::SendText(const FString& InputText, const TObjectPtr<ANPCBase>& NewNPC)
{
	CurNPC = NewNPC;
	CurNPC->StartConversation();

	HttpActor->SendText(CurNPC->GetNPCName(), InputText);
	ConversationUI->UpdateConversationUI(CurNPC->GetNPCName(), TEXT(""));
	ConversationUI->SetVisibility(ESlateVisibility::Visible);
}
#pragma endregion

#pragma region Talk to plant
void AFarmLifeGameMode::TalkToPlant(const FString& FileName, const FString& FilePath, const TArray<TObjectPtr<AActor>>& NewPlants)
{
	CurPlants = NewPlants;
	HttpActor->TalkToPlant(FileName, FilePath);
}

void AFarmLifeGameMode::SetTalkScore(int32 Score)
{
	TalkScore = Score;
	for(AActor* P : CurPlants)
	{
		// 점수 반응

	}
}

int32 AFarmLifeGameMode::GetTalkScore()
{
	return TalkScore;
}
#pragma endregion

#pragma region Time flow
void AFarmLifeGameMode::StartTime()
{
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AFarmLifeGameMode::UpdateMinutes, TIME_UPDATE_INTERVAL, true, TIME_UPDATE_INTERVAL);
}

void AFarmLifeGameMode::StopTime()
{
	GetWorldTimerManager().ClearTimer(TimerHandle);
}

void AFarmLifeGameMode::UpdateMinutes()
{
	Minutes += TEN_MINUTES;
	SunLight->AddActorWorldRotation(SunDeltaRot);
	if(Minutes == SIXTY_MINUTES)
	{
		UpdateHours();
	}
	TimerUI->UpdateTimerUI(Date, Hours, Minutes);
}

void AFarmLifeGameMode::UpdateHours()
{
	Minutes = 0;
	++Hours;

	// 시간 업데이트 일괄처리
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UHourUpdate::StaticClass(), OutActors);
	for(AActor* Actor : OutActors)
	{
		IHourUpdate* HU = Cast<IHourUpdate>(Actor);
		HU->OnHourUpdated(Hours);
	}

	if(Hours == END_HOUR)
	{
		UpdateDate();
	}
}

void AFarmLifeGameMode::UpdateDate()
{
	// 플레이어가 대화중이라면 보류


	Hours = START_HOUR;
	Minutes = 0;
	++Date;
	SunLight->SetActorRotation(SunBeginRot);

	// 날짜 업데이트 일괄처리
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UDateUpdate::StaticClass(), OutActors);
	for (AActor* Actor : OutActors)
	{
		IDateUpdate* HU = Cast<IDateUpdate>(Actor);
		HU->OnDateUpdated(Date);
	}
}
#pragma endregion

#pragma region TTS

#pragma endregion
