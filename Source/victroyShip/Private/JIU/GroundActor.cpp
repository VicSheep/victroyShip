// Fill out your copyright notice in the Description page of Project Settings.


#include "JIU/GroundActor.h"

#include "Components/BoxComponent.h"
#include "JIU/PlantActor.h"

// Sets default values
AGroundActor::AGroundActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	SetRootComponent(BoxComponent); // Set Component on Root Component
	BoxComponent->SetBoxExtent(FVector(180.f, 180.f, 32.f));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent); // Attach to Root Component
	MeshComponent->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, *PlanterPath));
}

// Called when the game starts or when spawned
void AGroundActor::BeginPlay()
{
	Super::BeginPlay();

	WaterFigure = 0.f;
	FertilizerFigure = 0.f;

	FTimerHandle timerHandle;
	GetWorldTimerManager().SetTimer(timerHandle, [&]() {
		if(Plant)
		{
			if (WaterFigure > 0.f)
			{
				WaterFigure -= 1.f;
			}
			if (FertilizerFigure > 0.f)
			{
				FertilizerFigure -= 1.f;
			}
		}
	}, 1.f, true);
}

// Called every frame
void AGroundActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGroundActor::PlantingSeed(int id)
{
	if (Plant == nullptr)
	{
		Plant = GetWorld()->SpawnActor<APlantActor>(APlantActor::StaticClass(), BoxComponent->GetComponentLocation(), FRotator(0.f));
		Plant->SetPlant(id, this);
	}
}

void AGroundActor::WaterPlant()
{
	if (Plant)
	{
		WaterFigure = 100.f;
	}
}

void AGroundActor::fertilizePlant()
{
	if (Plant)
	{
		FertilizerFigure = 100.f;
	}
}
