// Fill out your copyright notice in the Description page of Project Settings.


#include "OJS/PlayerMoveComp.h"
#include "OJS/PlayerCharacter.h"
#include "EnhancedInputComponent.h"

// Sets default values for this component's properties
UPlayerMoveComp::UPlayerMoveComp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPlayerMoveComp::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UPlayerMoveComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	Move();
}

void UPlayerMoveComp::SetupInput(UEnhancedInputComponent* input)
{
	if (nullptr == input)
		return;

	input->BindAction(iaMove, ETriggerEvent::Triggered, this, &UPlayerMoveComp::OnIAMove);
}

void UPlayerMoveComp::OnIAMove(const FInputActionValue& value)
{
	FVector2D vec = value.Get<FVector2D>();
	direction.X = vec.X;
	direction.Y = vec.Y;
}

void UPlayerMoveComp::Move()
{
	FTransform trans = me->GetActorTransform();
	me->AddMovementInput(trans.TransformVector(direction));

	direction = FVector::ZeroVector;
}

