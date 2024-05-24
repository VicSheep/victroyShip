// Fill out your copyright notice in the Description page of Project Settings.


#include "JIU/GroundActor.h"

#include "TimerManager.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "JIU/PlantActor.h"
#include "JIU/WeedActor.h"
#include "Math/UnrealMathUtility.h"
#include "UObject/ConstructorHelpers.h"

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

	ActorComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("ActorComponent"));
	ActorComponent->SetupAttachment(RootComponent);
	ActorComponent->SetChildActorClass(AWeedActor::StaticClass());

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> DefaultMaterialAsset(*DefaultMaterialPath);
	if (DefaultMaterialAsset.Succeeded())
	{
		DefaultMaterialInterface = DefaultMaterialAsset.Object;
		// MeshComponent->SetMaterial(0, DefaultMaterialInterface);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> DryMaterialAsset(*DryMaterialPath);
	if (DryMaterialAsset.Succeeded())
	{
		DryMaterialInterface = DryMaterialAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> WetMaterialAsset(*WetMaterialPath);
	if (WetMaterialAsset.Succeeded())
	{
		WetMaterialInterface = WetMaterialAsset.Object;
	}
}

// Called when the game starts or when spawned
void AGroundActor::BeginPlay()
{
	Super::BeginPlay();

	if (DefaultMaterialInterface)
	{
		MeshComponent->SetMaterial(0, DefaultMaterialInterface);
		GroundState = EGroundState::Default;
	}

	WeedActor = Cast<AWeedActor>(ActorComponent->GetChildActor());
	if (!WeedActor)
	{
		UE_LOG(LogTemp, Error, TEXT("No Weed"));
	}

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
			SetGroundMaterial();
		}
		else
		{
			RandomNumber = FMath::RandRange(1, 100);

			if (GroundState == EGroundState::DryPlanter)
			{
				if (RandomNumber <= 10)
				{
					GroundState = EGroundState::Default;
					MeshComponent->SetMaterial(0, DefaultMaterialInterface);
				}
			}
			else if (GroundState == EGroundState::Default && !isWeed)
			{
				if (RandomNumber <= 10)
				{
					isWeed = true;
					if (WeedActor)
					{
						WeedActor->SetVisible(true);
					}
				}
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
	if (Plant == nullptr && GroundState != EGroundState::Default)
	{
		Plant = GetWorld()->SpawnActor<APlantActor>(PlantFactory, BoxComponent->GetComponentLocation(), FRotator(0.f));
		Plant->SetPlant(id, this);
	}
}

void AGroundActor::WaterPlant()
{
	if (GroundState != EGroundState::Default)
	{
		WaterFigure = 100.f;
		SetGroundMaterial();
	}
}

void AGroundActor::FertilizePlant()
{
	if (GroundState != EGroundState::Default)
	{
		FertilizerFigure = 100.f;
		SetGroundMaterial();
	}
}

void AGroundActor::RemovePlant()
{
	if (Plant)
	{
		Plant->Destroy();
		Plant = nullptr;
	}
}

void AGroundActor::ProwGround()
{
	if (!isWeed)
	{
		if (GroundState == EGroundState::Default)
		{
			GroundState = EGroundState::DryPlanter;

			if (DryMaterialInterface)
			{
				MeshComponent->SetMaterial(0, DryMaterialInterface);
			}
		}
	}
}

void AGroundActor::RemoveWeed()
{
	if (isWeed && WeedActor)
	{
		isWeed = false;
		WeedActor->SetVisible(false);
	}
}

void AGroundActor::SetGroundMaterial()
{
	if (GroundState == EGroundState::DryPlanter)
	{
		if (WaterFigure > figureLimit && FertilizerFigure > figureLimit)
		{
			if (WetMaterialInterface)
			{
				MeshComponent->SetMaterial(0, WetMaterialInterface);
				GroundState = EGroundState::WetPlanter;
			}
		}
	}

	else if (GroundState == EGroundState::WetPlanter)
	{
		if (WaterFigure < figureLimit || FertilizerFigure < figureLimit)
		{
			if (DryMaterialInterface)
			{
				MeshComponent->SetMaterial(0, DryMaterialInterface);
				GroundState = EGroundState::DryPlanter;
			}
		}
	}
}
