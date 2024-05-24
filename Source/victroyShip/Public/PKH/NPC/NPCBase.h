// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NPCBase.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnLikeabilityChanged)

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

// Name
protected:
	UPROPERTY(EditAnywhere)
	FString NPCName;

public:
	FORCEINLINE void SetNPCName(const FString& NewName) { NPCName = NewName; }
	FORCEINLINE FString GetNPCName() const { return NPCName; }

// Likeability
protected:
	UPROPERTY(EditDefaultsOnly)
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
