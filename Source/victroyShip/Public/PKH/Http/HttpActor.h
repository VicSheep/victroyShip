// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/IHttpRequest.h"
#include "HttpActor.generated.h"

UCLASS()
class VICTROYSHIP_API AHttpActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AHttpActor();

protected:
	virtual void BeginPlay() override;

// NPC conversation
protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class AFarmLifeGameMode> MyGameMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString BaseURL = TEXT("http://127.0.0.1:8000");

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString EndPoint_SendSpeech = TEXT("/post-speech");

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString EndPoint_GetText = TEXT("/get-speech");

public:
	void SendSpeech(const FString& SpeechFileName, const FString& SpeechFilePath, const FString& NPCName);
	void SendSpeechComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

	void ReqTextFromSpeech();
	void ReqTextFromSpeechComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

// Talk to plant
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString EndPoint_TalkToPlant = TEXT("/post-talk2plant");

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString EndPoint_GetScore = TEXT("/get-talk2plant");

public:
	void TalkToPlant(const FString& SpeechFileName, const FString& SpeechFilePath);
	void TalkToPlantComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

	void ReqScore();
	void ReqScoreComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
};
