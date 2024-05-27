// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NPCBase.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnLikeabilityChanged)

UENUM()
enum class ENPCType : uint8
{
	Mira = 0,
	Junho,
	Chunsik
};

UCLASS()
class VICTROYSHIP_API ANPCBase : public ACharacter
{
	GENERATED_BODY()

public:
	ANPCBase();

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class ANPCController> NPCController;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class AFarmLifeGameMode> MyGameMode;

	UPROPERTY(EditAnywhere)
	ENPCType NPCType;

	UPROPERTY()
	TMap<FString, FString> NPCNameMap;

public:
	void StartConversation();
	void EndConversation();

// Animation
protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UNPCAnimInstance> AnimInstance;

// TTS 
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UMediaSoundComponent> MediaComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UMediaPlayer> MediaPlayer;

	UPROPERTY()
	FString CurEmotion;

	UFUNCTION()
	void OnPlayEnded();

public:
	void SetCurEmotion(const FString& NewEmotion);

	void PlayEmotion();

	void PlayTTS(const FString& FilePath);


// Talk To Player
protected:
	FString GreetingText = TEXT("안녕하세요!");
	FString GreetingEmotion = TEXT("joy");

	FString NPCTTSPath = TEXT("");
	bool IsRequestingTTS = false;

public:
	void RequestTTS();
	void SetTTSPath(const FString& NewTTSPath);

	void TalkToPlayer();


// Name
protected:
	UPROPERTY(EditAnywhere)
	FString NPCName;

public:
	FORCEINLINE void SetNPCName(const FString& NewName) { NPCName = NewName; }
	FORCEINLINE FString GetNPCName() const { return NPCName; }

// Likeability
protected:
	UPROPERTY(EditAnywhere)
	int32 CurLikeability = 0;

	UPROPERTY(EditDefaultsOnly)
	int32 MaxLikeability = 100;

public:
	FOnLikeabilityChanged OnLikeabilityChanged;

	void UpdateLikeability(int32 InLikeability);
	bool IsMaxLikeability();

	FORCEINLINE int32 GetLikeability() const { return CurLikeability; }

// Present
protected:
	int32 PreferItemId = 0;

	int32 NormalItemValue = 3;
	int32 PreferItemValue = 10;

public:
	void GivePresent(int32 NewItemId);
};
