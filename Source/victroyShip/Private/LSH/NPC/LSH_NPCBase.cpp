// Fill out your copyright notice in the Description page of Project Settings.


#include "LSH/NPC/LSH_NPCBase.h"

// Sets default values
ALSH_NPCBase::ALSH_NPCBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALSH_NPCBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALSH_NPCBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ALSH_NPCBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

