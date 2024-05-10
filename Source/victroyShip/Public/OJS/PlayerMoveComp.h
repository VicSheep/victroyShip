// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InputActionValue.h"
#include "OJS/PlayerBaseComp.h"
#include "PlayerMoveComp.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VICTROYSHIP_API UPlayerMoveComp : public UPlayerBaseComp
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerMoveComp();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void SetupInput(class UEnhancedInputComponent* input) override;

	UPROPERTY(EditDefaultsOnly)
	class UInputAction* iaMove;

	FVector direction;

	void OnIAMove(const FInputActionValue& value);

	void Move();



		
};
