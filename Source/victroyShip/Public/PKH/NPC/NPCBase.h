// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NPCBase.generated.h"

UCLASS()
class VICTROYSHIP_API ANPCBase : public ACharacter
{
	GENERATED_BODY()

public:
	ANPCBase();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

// TTS 출력
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UMediaSoundComponent> MediaComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UMediaPlayer> MediaPlayer;

	UFUNCTION()
	void OnPlayEnded();

public:
	void LoadSpeechFileAndPlay(const FString& FilePath);

// 이름
protected:
	UPROPERTY(EditAnywhere)
	FString NPCName;

public:
	FORCEINLINE void SetNPCName(const FString& NewName) { NPCName = NewName; }
	FORCEINLINE FString GetNPCName() const { return NPCName; }

// 호감도
protected:
	UPROPERTY(EditDefaultsOnly)
	int32 CurLikeability = 0;

	UPROPERTY(EditDefaultsOnly)
	int32 MaxLikeability = 5;

public:
	void LikeabilityUp(int32 InLikeability);
	bool IsMaxLikeability();

	FORCEINLINE int32 GetLikeability() const { return CurLikeability; }
};
