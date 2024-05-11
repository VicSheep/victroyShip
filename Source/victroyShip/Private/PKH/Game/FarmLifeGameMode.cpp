// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/Game/FarmLifeGameMode.h"

#include "PKH/Http/HttpActor.h"

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
}

void AFarmLifeGameMode::BeginPlay()
{
	Super::BeginPlay();

	if(HttpActorClass)
	{
		HttpActor = GetWorld()->SpawnActor<AHttpActor>(HttpActorClass);
	}

	//StartTime();
}

#pragma region NPC conversation
void AFarmLifeGameMode::SendSpeech(const FString& SpeechFileName, const FString& SpeechFilePath)
{
	HttpActor->SendSpeech(SpeechFileName, SpeechFilePath);
}

void AFarmLifeGameMode::SetLatestSpeech(const FString& SpeechText)
{
	LatestSpeech = SpeechText;
}

FString& AFarmLifeGameMode::GetLatestSpeech()
{
	return LatestSpeech;
}
#pragma endregion

#pragma region Talk to plant
void AFarmLifeGameMode::TalkToPlant(const FString& SpeechFileName, const FString& SpeechFilePath)
{
	HttpActor->TalkToPlant(SpeechFileName, SpeechFilePath);
}

void AFarmLifeGameMode::SetTalkScore(int32 Score)
{
	TalkScore = Score;
}

int32 AFarmLifeGameMode::GetTalkScore()
{
	return TalkScore;
}
#pragma endregion


#pragma region Time flow
void AFarmLifeGameMode::StartTime()
{
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AFarmLifeGameMode::UpdateMinutes, TIME_UPDATE_INTERVAL, false);
}

void AFarmLifeGameMode::StopTime()
{
	GetWorldTimerManager().ClearTimer(TimerHandle);
}

void AFarmLifeGameMode::UpdateMinutes()
{
	Minutes += TEN_MINUTES;
	if(Minutes == SIXTY_MINUTES)
	{
		UpdateHours();
	}
}

void AFarmLifeGameMode::UpdateHours()
{
	Minutes = 0;
	++Hours;

	// 시간 업데이트 일괄처리


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

	// 날짜 업데이트 일괄처리

}
#pragma endregion
