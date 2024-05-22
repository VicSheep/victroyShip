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

	// STT
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString EndPoint_SendSpeech = TEXT("/post-speech");

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString EndPoint_GetSpeech = TEXT("/get-speech");

	// ChatBot response
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString EndPoint_SendConv = TEXT("/post-conv");

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString EndPoint_GetConv = TEXT("/get-conv");

	// Direct text
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString EndPoint_SendText = TEXT("/post-text");

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString EndPoint_GetText = TEXT("/get-text");

	// TTS
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString EndPoint_GetTTS = TEXT("/get-tts");

	// End Chat
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString EndPoint_EndChat = TEXT("/end-chat");

public:
	// STT
	void SendSpeech(const FString& SpeechFileName, const FString& SpeechFilePath);
	void SendSpeechComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

	void ReqTextFromSpeech();
	void ReqTextFromSpeechComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

	// ChatBot response
	void SendConv(const FString& NPCName, int32 Preference);
	void SendConvComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

	void GetConv();
	void GetConvComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

	// Text
	void SendText(const FString& NPCName, const FString& InputText, int32 Preference);
	void SendTextComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

	void GetText();
	void GetTextComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

	// TTS
	void GetTTS();
	void GetTTSComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

	// End Chat
	void EndChat(const FString& NPCName);
	void EndChatComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

// Talk to plant
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString EndPoint_TalkToPlant = TEXT("/post-talk2plant");

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString EndPoint_GetSpeechScore = TEXT("/get-talk2plant");

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString EndPoint_TextToPlant = TEXT("/post-text2plant");

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString EndPoint_GetTextScore = TEXT("/get-text2plant");

public:
	// STT
	void TalkToPlant(const FString& SpeechFileName, const FString& SpeechFilePath);
	void TalkToPlantComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

	void ReqScore();
	void ReqScoreComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

	// Text
	void TalkToPlantWithText(const FString& InputText);
	void TalkToPlantWithTextComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

	void ReqScoreWithText();
	void ReqScoreWithTextComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
};
