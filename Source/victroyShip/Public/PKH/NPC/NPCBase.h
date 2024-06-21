// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PKH/Game/FarmLifeGameMode.h"
#include "PKH/Interface/DateUpdate.h"
#include "NPCBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLikeabilityChanged, int32, NPCIndex);

UENUM()
enum class ENPCType : uint8
{
	Artist = 0,
	Cafe,
	Farmer,
	Fisherman,
	Unemployed,
	Programmer
};

UENUM()
enum class EEmotion : uint8
{
	none = 0,
	joy,
	surprise,
	sadness,
	anger,
	noticed
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
	void RunAI();
	void StopAI();

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
	FString Name_Jimin = TEXT("김지민");

	UPROPERTY(EditDefaultsOnly)
	FString Name_Ayeong = TEXT("민아영");

	UPROPERTY(EditDefaultsOnly)
	FString Name_Chawon = TEXT("박채원");

	UPROPERTY(EditDefaultsOnly)
	FString Name_Minwoo = TEXT("성민우");

	UPROPERTY(EditDefaultsOnly)
	FString Name_Junho = TEXT("이준호");

	UPROPERTY(EditDefaultsOnly)
	FString Name_Chunsik = TEXT("이춘식");

	UPROPERTY(EditDefaultsOnly)
	TMap<ENPCType, FString> NPCNameMap;

public:
	virtual void StartConversation(bool IsStart);
	virtual void OnConversationBegin();

	virtual void EndConversation();
	virtual void OnConversationEnd();

// Animation
protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UNPCAnimInstance> AnimInstance;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UAnimMontage> Montage_Conv;

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

	virtual void PlayEmotion(bool IsUIOnly = false);

	void PlayTTS(const FString& FilePath);

// Speed
protected:
	UPROPERTY(EditAnywhere, Category = "Speed")
	float PatrolSpeed = 100.0f;

	UPROPERTY(EditAnywhere, Category="Speed")
	float WalkSpeed = 250.0f;

	UPROPERTY(EditAnywhere, Category = "Speed")
	float RunSpeed = 300.0f;

public:
	void SetNPCPatrol();
	void SetNPCWalk();
	void SetNPCRun();

// Greeting
protected:
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

	UFUNCTION(BlueprintCallable)
	FORCEINLINE FString GetNPCName() const { return NPCName; }

// Likeability
protected:
	UPROPERTY(EditAnywhere, Category="likeability")
	int32 CurLikeability = 0;

	UPROPERTY(EditDefaultsOnly, Category = "likeability")
	int32 FriendlyLikeability = 50;

	UPROPERTY(EditDefaultsOnly, Category = "likeability")
	int32 MaxLikeability = 100;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UNiagaraComponent> VfxComp;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UNiagaraSystem> Vfx_LikeUp;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UNiagaraSystem> Vfx_LikeDown;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UNiagaraSystem> Vfx_CurLike;

public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnLikeabilityChanged OnLikeabilityChanged;

	void UpdateLikeability(int32 InLikeability);
	bool IsMaxLikeability();

	UFUNCTION(BlueprintCallable)
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
	bool GetPresentToday = false;
	bool IsPreferItem = false;

	UPROPERTY(EditDefaultsOnly)
	FString PreferItemName = TEXT("");

	int32 NormalItemValue = 5;
	int32 PreferItemValue = 15;

public:
	UFUNCTION(BlueprintCallable)
	void GivePresent(const FString& ItemName);

	void ResponseToPresent();

// Job
protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UAnimMontage> Montage_Work;

	UPROPERTY(EditDefaultsOnly)
	FRotator WorkRotation;

public:
	virtual void DoJob();

	virtual void StartSit();

	virtual void EndSit();

	virtual void StandUp();

	virtual bool CanRotateInWorking();

// Portraits
protected:
	UPROPERTY(EditDefaultsOnly)
	TArray<TObjectPtr<class UTexture2D>> Portraits_Joy;

	UPROPERTY(EditDefaultsOnly)
	TArray<TObjectPtr<class UTexture2D>> Portraits_Surprise;

	UPROPERTY(EditDefaultsOnly)
	TArray<TObjectPtr<class UTexture2D>> Portraits_Sadness;

	UPROPERTY(EditDefaultsOnly)
	TArray<TObjectPtr<class UTexture2D>> Portraits_Anger;

	UPROPERTY(EditDefaultsOnly)
	TArray<TObjectPtr<class UTexture2D>> Portraits_Default;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UTexture2D> CurPortrait;

	void SetCurPortrait();

// Sound
protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class USoundBase> Sfx_Notice;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class USoundBase> Sfx_Joy;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class USoundBase> Sfx_Anger;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class USoundBase> Sfx_Sad;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class USoundBase> Sfx_Surprise;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class USoundBase> Sfx_Indiff;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UAudioComponent> SfxComp;

// ETC
protected:
	UPROPERTY(EditAnywhere)
	float SitDistance = 0;

// Interface
public:
	virtual void OnDateUpdated(int32 NewDate) override;

};
