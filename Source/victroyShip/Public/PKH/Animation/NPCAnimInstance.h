// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "NPCAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class VICTROYSHIP_API UNPCAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UNPCAnimInstance();

	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY()
	TObjectPtr<class ACharacter> OwningNPC;

	UPROPERTY()
	TObjectPtr<class UCharacterMovementComponent> MoveComp;

protected:
	float WalkThreshold = 5.0f;
	float RunThreshold = 210.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bIsIdle : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bIsWalking : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bIsRunning : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bIsFalling : 1;

// Montage
protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UAnimMontage> Montage_Conv;

	UPROPERTY(EditDefaultsOnly)
	TArray<TObjectPtr<class UAnimMontage>> EmotionMontages;

	UPROPERTY(EditDefaultsOnly)
	TMap<FString, int32> EmotionMap;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UAnimMontage> Montage_Indiff;

public:
	void PlayMontage_Conv(float PlayRate = 1.0f);

	void PlayMontage_Emotion(const FString& Emotion, float PlayRate = 1.0f);

	void PlayMontage_Custom(class UAnimMontage* NewMontage, float PlayRate = 1.0f);

	void StopSpecificMontage(class UAnimMontage* TargetMontage, float BlendOutTime = 0.0f);
};
