// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/Test/STTTestCharacter.h"

#include "AudioCaptureComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

ASTTTestCharacter::ASTTTestCharacter()
{
	RecordComp = CreateDefaultSubobject<UAudioCaptureComponent>(TEXT("RecordComp"));
}

void ASTTTestCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void ASTTTestCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

