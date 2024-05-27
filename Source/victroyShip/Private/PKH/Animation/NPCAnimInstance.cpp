// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/Animation/NPCAnimInstance.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UNPCAnimInstance::UNPCAnimInstance()
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage> Montage_ConvRef(TEXT("/Script/Engine.AnimMontage'/Game/PKH/Anim/AM_Conversation.AM_Conversation'"));
	if(Montage_ConvRef.Object)
	{
		Montage_Conv = Montage_ConvRef.Object;
	}

	// Mapping
	EmotionMap.Add(TEXT("joy"), 0);
	EmotionMap.Add(TEXT("sadness"), 1);
	EmotionMap.Add(TEXT("surprise"), 2);
	EmotionMap.Add(TEXT("anger"), 3);
	EmotionMap.Add(TEXT("neutral"), 1);
	EmotionMap.Add(TEXT("indifference"), 2);
	EmotionMap.Add(TEXT("curiosity"), 3);

	// Emotion Montages
	static ConstructorHelpers::FObjectFinder<UAnimMontage> Montage_Joy(TEXT("/Script/Engine.AnimMontage'/Game/PKH/Anim/AM_Joyful.AM_Joyful'"));
	if (Montage_Joy.Object)
	{
		EmotionMontages.Add(Montage_Joy.Object);
	}
	static ConstructorHelpers::FObjectFinder<UAnimMontage> Montage_Sadness(TEXT("/Script/Engine.AnimMontage'/Game/PKH/Anim/AM_Sad.AM_Sad'"));
	if (Montage_Sadness.Object)
	{
		EmotionMontages.Add(Montage_Sadness.Object);
	}
	static ConstructorHelpers::FObjectFinder<UAnimMontage> Montage_Surprise(TEXT("/Script/Engine.AnimMontage'/Game/PKH/Anim/AM_Surprised.AM_Surprised'"));
	if (Montage_Surprise.Object)
	{
		EmotionMontages.Add(Montage_Surprise.Object);
	}
	static ConstructorHelpers::FObjectFinder<UAnimMontage> Montage_Anger(TEXT("/Script/Engine.AnimMontage'/Game/PKH/Anim/AM_Angry.AM_Angry'"));
	if (Montage_Anger.Object)
	{
		EmotionMontages.Add(Montage_Anger.Object);
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> Montage_IndiffRef(TEXT("/Script/Engine.AnimMontage'/Game/PKH/Anim/AM_Indifference.AM_Indifference'"));
	if (Montage_IndiffRef.Object)
	{
		Montage_Indiff = Montage_IndiffRef.Object;
	}
}

void UNPCAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwningNPC = Cast<ACharacter>(GetOwningActor());
	if(OwningNPC)
	{
		MoveComp = OwningNPC->GetCharacterMovement();
	}
}

void UNPCAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (nullptr == MoveComp)
	{
		return;
	}

	bIsIdle = MoveComp->Velocity.Size2D() < WalkThreshold;
	bIsWalking = MoveComp->Velocity.Size2D() > WalkThreshold;
	bIsRunning = MoveComp->Velocity.Size2D() > RunThreshold;
	bIsFalling = MoveComp->IsFalling();
}

void UNPCAnimInstance::PlayMontage_Conv()
{
	StopAllMontages(0);
	Montage_Play(Montage_Conv, 1.0f);
}

void UNPCAnimInstance::PlayMontage_Emotion(const FString& Emotion)
{
	if(false == EmotionMap.Contains(Emotion))
	{
		UE_LOG(LogTemp, Warning, TEXT("There is no emotion: %s"), *Emotion);
		Montage_Play(Montage_Indiff, 1.0f);
		return;
	}

	const int32 Idx = EmotionMap[Emotion];
	Montage_Play(EmotionMontages[Idx], 1.0f);
}
