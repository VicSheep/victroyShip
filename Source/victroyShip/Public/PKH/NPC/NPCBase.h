// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PKH/Game/FarmLifeGameMode.h"
#include "PKH/Interface/DateUpdate.h"
#include "NPCBase.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnLikeabilityChanged)

UENUM()
enum class ENPCType : uint8
{
	Mira = 0,
	Junho,
	Chunsik
};

UENUM()
enum class EEmotion : uint8
{
	none = 0,
	joy,
	surprise,
	sad,
	anger
};

UCLASS()
class VICTROYSHIP_API ANPCBase : public ACharacter, public IDateUpdate
{
	GENERATED_BODY()

public:
	ANPCBase();

protected:
	virtual void BeginPlay() override;
	
public:
	virtual void Tick(float DeltaSeconds) override;

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UWidgetComponent> EmotionUIComp;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class ANPCController> NPCController;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class AFarmLifeGameMode> MyGameMode;

	UPROPERTY(EditAnywhere)
	ENPCType NPCType;

	UPROPERTY(EditAnywhere, Category="Locations")
	FVector HomeLoc = FVector();

// For Mapping
protected:
	// Keys For Map
	UPROPERTY(EditDefaultsOnly)
	FString Name_Mira = TEXT("미라");

	UPROPERTY(EditDefaultsOnly)
	FString Name_Junho = TEXT("이준호");

	UPROPERTY(EditDefaultsOnly)
	FString Name_Chunsik = TEXT("이춘식");

	UPROPERTY(EditDefaultsOnly)
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

	UPROPERTY(VisibleAnywhere)
	FString CurEmotion;

	UFUNCTION()
	void OnPlayEnded();

public:
	void SetCurEmotion(const FString& NewEmotion);
	void SetCurEmotion(EEmotion NewEmotion);

	void PlayEmotion();

	void PlayTTS(const FString& FilePath);

// Speed
protected:
	UPROPERTY(EditDefaultsOnly, Category="Speed")
	float WalkSpeed = 200.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Speed")
	float RunSpeed = 270.0f;

public:
	void SetNPCWalk();
	void SetNPCRun();

// Greeting
protected:
	//FString GreetingText = TEXT("안녕하세요, 인사 한번만 해주세요!");
	bool HasIntendToGreeting = false;

public:
	void InitGreeting();

	void GreetingToPlayer();

// Name
protected:
	UPROPERTY(EditAnywhere)
	FString NPCName;

public:
	FORCEINLINE void SetNPCName(const FString& NewName) { NPCName = NewName; }
	FORCEINLINE FString GetNPCName() const { return NPCName; }

// Likeability
protected:
	UPROPERTY(EditAnywhere, Category="likeability")
	int32 CurLikeability = 0;

	UPROPERTY(EditDefaultsOnly, Category = "likeability")
	int32 FriendlyLikeability = 50;

	UPROPERTY(EditDefaultsOnly, Category = "likeability")
	int32 MaxLikeability = 100;

public:
	FOnLikeabilityChanged OnLikeabilityChanged;

	void UpdateLikeability(int32 InLikeability);
	bool IsMaxLikeability();

	FORCEINLINE int32 GetLikeability() const { return CurLikeability; }
	bool IsFriendly() const;

// Emotion
protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UEmotionUIWidget> EmotionUI;

public:
	void SetEmotionUI(bool IsActive);

// Present
protected:
	int32 PreferItemId = 0;

	int32 NormalItemValue = 5;
	int32 PreferItemValue = 15;

	UPROPERTY(EditDefaultsOnly)
	FString PresentText = TEXT("이거 선물이야, 받아줘!");

public:
	void GivePresent(int32 NewItemId);

// Job
protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UAnimMontage> Montage_Work;

	UPROPERTY(EditDefaultsOnly)
	FRotator WorkRotation;

public:
	virtual void DoJob();

// Interface
public:
	virtual void OnDateUpdated(int32 NewDate) override;

};
