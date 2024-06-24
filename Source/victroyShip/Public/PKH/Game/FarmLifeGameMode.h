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

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFarmLifeStart);

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
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// 튜토리얼 대응
public:
	UFUNCTION(BlueprintCallable)
	void StartFarmLife();

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnFarmLifeStart OnFarmLifeStart;

	// Player & NPC
protected:
	UPROPERTY(VisibleAnywhere)
	FVector PlayerHomLoc;

	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<class ANPCBase>> NPCArray;

	// Http
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ANewHttpActor> HttpActorClass;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class ANewHttpActor> HttpActor;

	// STT
protected:
	FString LatestSpeech;
	int32 TalkScore;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class ANPCBase> CurNPC;

	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<class APlantActor>> CurPlants;

	UPROPERTY(VisibleAnywhere)
	FString CurTTSFilePath = TEXT("");

public:
	UFUNCTION(BlueprintCallable)
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
	void PlayReservedTTS();

	// Talk to plant
	void TalkToPlant(const FString& FileName, const FString& FilePath, const TArray<TObjectPtr<class APlantActor>>& NewPlants);

	void SetTalkScore(int32 Score);
	int32 GetTalkScore();

	void TalkToPlantWithText(const FString& InputText, const TArray<TObjectPtr<class APlantActor>>& NewPlants);

	bool IsInConversation();

	// Greeting
public:
	void InitGreeting(const FString& NPCName, int32 Likeability);

	void RequestGreetingData(class ANPCBase* NewNPC);

	void GreetingToPlayer(const FNPCResponse& NPCResponse);

	// Present
public:
	void RequestPresentData(class ANPCBase* NewNPC, const FString& ItemName, bool IsPrefer);

	void ResponseToPlayerForPresent(const FNPCResponse& NPCResponse);

	// Time flow
protected:
	FTimerHandle TimerHandle;

	int32 Hours = 8;
	int32 Minutes = 0;

	UPROPERTY(EditDefaultsOnly)
	int32 Date = 0;

	bool Paused = false;

	UPROPERTY(EditAnywhere)
	float TimeUpdateInterval = 5.0f;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<AActor> SunLight;

	const FRotator SunBeginRot = FRotator(220, 0, 0);
	const FRotator SunEndRot = FRotator(360, 0, 0);
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

	FORCEINLINE int32 GetHour() const { return Hours; };

	// InputMode
protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class APlayerController> MyController;

	FInputModeGameOnly InputMode_Game;
	FInputModeUIOnly InputMode_UI;
	FInputModeGameAndUI InputMode_Both;

public:
	UFUNCTION(BlueprintCallable)
	void ChangeInputMode_Game();

	UFUNCTION(BlueprintCallable)
	void ChangeInputMode_UI();

	UFUNCTION(BlueprintCallable)
	void ChangeInputMode_Both();

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

public:
	void UpdatePortrait(class UTexture2D* NewPortrait);

	void RecordOn();
	void RecordOff();

	UFUNCTION(BlueprintCallable)
	bool CanTalkOrPresent();

	// Sound
protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class USoundBase> BGM_BackToPortland;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UAudioComponent> BGMComp;

	UPROPERTY(EditDefaultsOnly)
	float BgmMultiplier_Normal = 1.0f;

	UPROPERTY(EditDefaultsOnly)
	float BgmMultiplier_Conv = 0.6f;

// StartScreen
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> StartUIClass;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UUserWidget> StartUI;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class ULevelSequence> StartSequence;

	UPROPERTY(VisibleAnywhere)
	class ALevelSequenceActor* StartSequenceActor;

	UPROPERTY(VisibleAnywhere)
	class ULevelSequencePlayer* StartSequencePlayer;

	void StartScreenOn();
	void StartScreenOff();

// Tutorial
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> TutorialUIClass;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UUserWidget> TutorialUI;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> GuideUIClass;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UUserWidget> GuideUI;

public:
	UFUNCTION(BlueprintCallable)
	void StartTutorial();

// Ending
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UEndingUI_Success> EndingUI_SuccessClass;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UEndingUI_Success> EndingUI_Success;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class USoundBase> BGM_EndingSuccess;

	// Threshold
	UPROPERTY(EditDefaultsOnly)
	int32 EndingSuccessCount = 3;

	UPROPERTY(EditDefaultsOnly)
	int32 EndingSuccessLikeability = 80;

	void EndGame();

public:
	void CheckEndingCondition();

};
