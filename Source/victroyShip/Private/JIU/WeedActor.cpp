// Fill out your copyright notice in the Description page of Project Settings.


#include "JIU/WeedActor.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AWeedActor::AWeedActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	SetRootComponent(BoxComp); // Set Component on Root Component
	BoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	MeshComp1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent1"));
	MeshComp1->SetupAttachment(RootComponent);
	MeshComp1->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, *MeshPath1));
	MeshComp1->SetRelativeLocation(FVector(-50.f, 20.f, 0.f));
	MeshComp1->SetVisibility(false);
	MeshComp1->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	MeshComp2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent2"));
	MeshComp2->SetupAttachment(RootComponent);
	MeshComp2->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, *MeshPath2));
	MeshComp2->SetRelativeLocation(FVector(60.f, 40.f, 0.f));
	MeshComp2->SetVisibility(false);
	MeshComp2->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	MeshComp3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent3"));
	MeshComp3->SetupAttachment(RootComponent);
	MeshComp3->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, *MeshPath3));
	MeshComp3->SetRelativeLocation(FVector(30.f, -60.f, 0.f));
	MeshComp3->SetVisibility(false);
	MeshComp3->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void AWeedActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWeedActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeedActor::SetVisible(bool isVisible)
{
	MeshComp1->SetVisibility(isVisible);
	MeshComp2->SetVisibility(isVisible);
	MeshComp3->SetVisibility(isVisible);
}
