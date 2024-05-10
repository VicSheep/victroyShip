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

// Request & Response
protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class AFarmLifeGameMode> GameMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString BaseURL = TEXT("http://127.0.0.1:8000");

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString EndPoint_SendSpeech = TEXT("/post-speech");

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString EndPoint_GetText = TEXT("/get-text");

public:
	void SendSpeech(const FString& SpeechFileName, const FString& SpeechFilePath);
	void SendSpeechComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

	void ReqTextFromSpeech();
	void ReqTextFromSpeechComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
};
