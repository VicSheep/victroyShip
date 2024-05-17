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
