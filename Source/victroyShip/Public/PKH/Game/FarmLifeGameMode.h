﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FarmLifeGameMode.generated.h"

class UNPCConversationWidget;
/**
 * 
 */
UCLASS()
class VICTROYSHIP_API AFarmLifeGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AFarmLifeGameMode();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaSeconds) override;

// Http
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AHttpActor> HttpActorClass;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class AHttpActor> HttpActor;

// STT
protected:
	FString LatestSpeech;
	int32 TalkScore;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class ANPCBase> CurNPC;

	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<class APlantActor>> CurPlants;

// Initialize
protected:
	const FString& InitialName = TEXT("Speech");
	const FString& InitialPath = TEXT("D:/Projects/victroyShip/Saved/BouncedWavFiles/Speech.wav");
	const FString& InitialNPC = TEXT("미라");

	void Initialize();

// Conversation
public:
	// NPC conversation
	void SendSpeech(const FString& FileName, const FString& FilePath, const TObjectPtr<class ANPCBase>& NewNPC);

	void SetLatestSpeech(const FString& Response, const FString& FilePath);
	FString& GetLatestSpeech();

	void EndConversation();

	// NPC conversation by text
	void SendText(const FString& InputText, const TObjectPtr<class ANPCBase>& NewNPC);

	// Talk to plant
	void TalkToPlant(const FString& FileName, const FString& FilePath, const TArray<TObjectPtr<class APlantActor>>& NewPlants);

	void SetTalkScore(int32 Score);
	int32 GetTalkScore();

	void TalkToPlantWithText(const FString& InputText, const TArray<TObjectPtr<class APlantActor>>& NewPlants);

// Time flow
protected:
	FTimerHandle TimerHandle;

	int32 Hours = 8;
	int32 Minutes = 0;
	int32 Date = 1;
	bool Paused = false;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class ADirectionalLight> SunLight;

	const FRotator SunBeginRot = FRotator(200, 0, 0);
	const FRotator SunEndRot = FRotator(350, 0, 0);
	const FRotator SunDeltaRot = FRotator(0.5f, 0, 0);

	UFUNCTION()
	void UpdateMinutes();

	void UpdateHours();
	void UpdateDate();

public:
	FORCEINLINE bool IsPausedNow() const { return Paused; }

	void StartTime();
	void StopTime();

	UFUNCTION()
	void OnNextDay();

	UFUNCTION()
	void OnFadeOutFinished();

	void CheckDateUpdate();

// TTS
protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USoundWave> SpeechSound;

// UI
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UNPCConversationWidget> ConversationUIClass;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UNPCConversationWidget> ConversationUI;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UTimerWidget> TimerUIClass;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UTimerWidget> TimerUI;
};
