// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/IHttpRequest.h"
#include "ServerTestActor.generated.h"

UCLASS()
class VICTROYSHIP_API AServerTestActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AServerTestActor();

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<class UMediaSoundComponent> MediaComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UMediaPlayer> MediaPlayer;

	void PlayMediaSound(const FString& FilePath);

	UFUNCTION()
	void OnPlayEnded();

// wav only
public:
	// Post
	void SendWavFile(const FString& FilePath);
	void SendWavFileComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

	// Get
	void GetWavFile();
	void GetWavFileComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

// STT
public:
	// Post
	void Post_STT(const FString& FilePath);
	void Post_STTComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

	// Get
	void Get_STT();
	void Get_STTComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

// TTS
public:
	// Post
	void Post_TTS(const FString& NPCName, const FString& NPCChat);
	void Post_TTSComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

	// Get
	void Get_TTS();
	void Get_TTSComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
};
