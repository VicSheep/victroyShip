// Fill out your copyright notice in the Description page of Project Settings.


#include "OJS/BaseInventorySystem/MasterPickupActor.h"

// Sets default values
AMasterPickupActor::AMasterPickupActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMasterPickupActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMasterPickupActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

