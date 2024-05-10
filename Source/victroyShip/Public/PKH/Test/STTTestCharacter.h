// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "victroyShip/victroyShipCharacter.h"
#include "STTTestCharacter.generated.h"

/**
 * 
 */
UCLASS()
class VICTROYSHIP_API ASTTTestCharacter : public AvictroyShipCharacter
{
	GENERATED_BODY()

public:
	ASTTTestCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

// Component
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UAudioCaptureComponent> RecordComp;
};
