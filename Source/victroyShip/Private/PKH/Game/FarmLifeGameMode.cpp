// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/Game/FarmLifeGameMode.h"

#include "PKH/Http/HttpActor.h"

AFarmLifeGameMode::AFarmLifeGameMode()
{


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

	/*FTimerHandle Handle;
	GetWorldTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([this]()
	{
		HttpActor->SendSpeech(TEXT("Http Test"));
	}), 3.0f, false);*/
}

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
