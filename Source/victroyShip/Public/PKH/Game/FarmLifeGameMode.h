// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FarmLifeGameMode.generated.h"

USTRUCT()
struct FNPCResponse
{
	GENERATED_BODY()

	FString Answer;
	FString Emotion;
	FString FilePath;
	int32 Likeability;
};

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

public:
	FORCEINLINE class ANPCBase* GetCurNPC() const { return CurNPC; }

// Conversation
public:
	// NPC conversation
	void SendSpeech(const FString& FileName, const FString& FilePath, const TObjectPtr<class ANPCBase>& NewNPC);

	void SetLatestSpeech(const FNPCResponse& Response);
	FString& GetLatestSpeech();

	void EndConversation();

	// Player input text
	void ShowPlayerText(const FString& PlayerInputText);

	// NPC conversation by text
	void SendText(const FString& InputText, const TObjectPtr<class ANPCBase>& NewNPC);

	// Emotion
	void PlayNPCEmotion();

	// TTS
	void PlayTTS(const FString& FilePath);

	// Talk to plant
	void TalkToPlant(const FString& FileName, const FString& FilePath, const TArray<TObjectPtr<class APlantActor>>& NewPlants);

	void SetTalkScore(int32 Score);
	int32 GetTalkScore();

	void TalkToPlantWithText(const FString& InputText, const TArray<TObjectPtr<class APlantActor>>& NewPlants);

// TalkFromNPC
public:
	void RequestTTS(class ANPCBase* NewNPC, const FString& InputText);

	void SetNPCTTS(const FString& NewTTSPath);

	void TalkToPlayer(const FString& InputText, const FString& NewEmotion);

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
