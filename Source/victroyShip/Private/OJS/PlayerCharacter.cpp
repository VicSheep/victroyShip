// Fill out your copyright notice in the Description page of Project Settings.


#include "OJS/PlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "EnhancedInputComponent.h"
#include "OJS/PlayerMoveComp.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	moveComp = CreateDefaultSubobject<UPlayerMoveComp>(TEXT("MoveComp"));

	boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	this->SetRootComponent(boxComp);
	boxComp->SetBoxExtent(FVector(30.0f, 30.0f ,60.0f));

	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	meshComp->SetupAttachment(RootComponent);

	

}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	SetupInputDelegate.Broadcast(input);
}

