// Fill out your copyright notice in the Description page of Project Settings.


#include "JIU/PlantActor.h"

#include "Components/BoxComponent.h"
#include "Engine/Engine.h"
#include "JIU/GroundActor.h"

// Sets default values
APlantActor::APlantActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	SetRootComponent(BoxComponent); // Set Component on Root Component

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent); // Attach to Root Component
}

// Called when the game starts or when spawned
void APlantActor::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void APlantActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// UE_LOG(LogTemp, Warning, TEXT("%f"), WaterFigure);
}

void APlantActor::Destroyed()
{
	Super::Destroyed();

	if (Ground)
	{
		if (Ground->Plant)
		{
			Ground->Plant = nullptr;
		}
	}
}

void APlantActor::SetPlant(int id, AGroundActor* _ground)
{
	state = 0;
	this->Ground = _ground;

	switch (id)
	{
	case 0:
		MeshComponent->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, *GrapePath0));
		PlantType = EPlantType::Grape;
		state = 4;
		break;
	case 1:
		PlantType = EPlantType::Sunflower;
		state = 3;
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("Mesh not set."));
	}
}

void APlantActor::GrowPlant()
{
	if (Ground->WaterFigure < 20.f || Ground->FertilizerFigure < 20.f)	return;

	switch (PlantType)
	{
	case EPlantType::Grape:
		{
			if (state == 4)
			{
				state = 3;
				MeshComponent->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, *GrapePath1));
			}
			else if (state == 3)
			{
				state = 2;
				MeshComponent->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, *GrapePath2));
			}
			else if (state == 2)
			{
				state = 1;
				MeshComponent->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, *GrapePath3));
			}
			else if (state == 1)
			{
				state = 0;
				MeshComponent->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, *GrapePath4));
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "Havest Grape");
			}
			break;
		}
	case EPlantType::Sunflower:
		{
			if (state == 3)
			{
				state = 2;
				MeshComponent->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, *SunflowerPath1));
			}
			else if (state == 2)
			{
				state = 1;
				MeshComponent->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, *SunflowerPath2));
			}
			else if (state == 1)
			{
				state = 0;
				MeshComponent->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, *SunflowerPath3));
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "Havest Sunflower");
			}
			break;
		}
	default:
		UE_LOG(LogTemp, Warning, TEXT("Plant type error."));
	}
}

void APlantActor::HavestPlant()
{
	if (state == 0)
	{
		switch (PlantType)
		{
		case EPlantType::Grape:
		{
			
		}
		case EPlantType::Sunflower:
		{
			
		}
		default:
			UE_LOG(LogTemp, Warning, TEXT("Plant type error."));
		}
	}
}
