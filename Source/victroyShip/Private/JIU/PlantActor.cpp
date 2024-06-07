// Fill out your copyright notice in the Description page of Project Settings.


#include "JIU/PlantActor.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "Curves/CurveFloat.h"
#include "Engine/Engine.h"
#include "JIU/GroundActor.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
APlantActor::APlantActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	SetRootComponent(BoxComponent); // Set Component on Root Component

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent); // Attach to Root Component

	static ConstructorHelpers::FObjectFinder<UDataTable> DataTable(*PlantDataTablePath);
	if (DataTable.Succeeded())
	{
		PlantDataTable = DataTable.Object;
	}

	GrowParticleSystem = LoadObject<UParticleSystem>(nullptr, TEXT("/Game/JIU/Effects/P_Sparks_E.P_Sparks_E"));
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

	MyTimeline.TickTimeline(DeltaTime);

	// UE_LOG(LogTemp, Error, TEXT("Plant State : %d"), CurLevel);
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

FPlantStruct APlantActor::GetPlantData(FName RowName)
{
	if (PlantDataTable)
	{
		static const FString ContextString(TEXT("Plant Data Context"));

		// RowName을 사용하여 특정 행을 가져옵니다.
		FPlantStruct* Row = PlantDataTable->FindRow<FPlantStruct>(RowName, ContextString);

		if (!Row)
		{
			// UE_LOG(LogTemp, Error, TEXT("Row with name '%s' not found"), *RowName.ToString());
			return FPlantStruct();
		}

		return *Row;
	}

	UE_LOG(LogTemp, Error, TEXT("Data Table Error"));
	return FPlantStruct();
}

void APlantActor::SetPlant(int id, AGroundActor* _ground)
{
	this->Ground = _ground;
	PlantState = EPlantState::Seed;

	if (PlantDataTable)
	{
		plantID = id;
		PlantInfo = GetPlantData(FName(FString::FromInt(id)));
	}
	
	if (PlantInfo.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("You Seed '%s'"), *PlantInfo.Name);
		MaxGrowLevel = PlantInfo.GrowLevel;

		if (PlantInfo.Bamboo)
		{
			MeshComponent->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, *PlantInfo.SeedPath));
		}

		if (PlantInfo.HavestType == EHavestType::Many)
		{
			isRepeated = true;
			MaxHavestLevel = PlantInfo.HavestLevel;
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Plant Data Table Error"));
	}
}

void APlantActor::GrowPlant()
{
	if (Ground->WaterFigure < 20.f || Ground->FertilizerFigure < 20.f)	return;

	CurLevel += 1;
	haveChange = true;

	if (PlantState == EPlantState::Seed)
	{
		PlantState = EPlantState::Growing;
		NewMesh = LoadObject<UStaticMesh>(nullptr, *PlantInfo.GetPath(PlantState == EPlantState::Growing ? true : false, CurLevel));
		StartScaling();
	}
	else if (PlantState == EPlantState::Growing)
	{
		if (CurLevel > MaxGrowLevel)
		{
			PlantState = EPlantState::Mature;

			NewMesh = LoadObject<UStaticMesh>(nullptr, *PlantInfo.MaturePath);
			StartScaling();
		} else
		{
			NewMesh = LoadObject<UStaticMesh>(nullptr, *PlantInfo.GetPath(PlantState == EPlantState::Growing ? true : false, CurLevel));
			StartScaling();
		}
	}
	else if (PlantState == EPlantState::Mature)
	{
	}
	else if (PlantState == EPlantState::Havested)
	{
		if (CurLevel > MaxHavestLevel)
		{
			PlantState = EPlantState::Mature;

			NewMesh = LoadObject<UStaticMesh>(nullptr, *PlantInfo.MaturePath);
			StartScaling();
		}
	}
}

void APlantActor::StartScaling()
{
	MeshComponent->SetWorldScale3D(InitialScale);

	if (FloatCurve)
	{
		SetupTimeline();
		MyTimeline.PlayFromStart();
	}
}

void APlantActor::HandleProgress(float Value)
{
	// 일시적인 스케일 변화 설정
	FVector TempMaxScale = (MaxScale == InitialScale) ? InitialScale + FVector(0.3f, 0.3f, 0.3f) : MaxScale;

	FVector NewScale = FMath::Lerp(InitialScale, TempMaxScale, Value);

	if (Value >= 0.85f && isChanged)
	{
		if (NewMesh)
		{
			MeshComponent->SetStaticMesh(NewMesh);
			isChanged = false;

			if (PlantState == EPlantState::Mature)
			{
				SpawnPaticleSystem(GrowParticleSystem);
			}
		}
	}

	MeshComponent->SetWorldScale3D(NewScale);
}

void APlantActor::OnTimelineFinished()
{
	NewMesh = nullptr;
	isChanged = true;
}

void APlantActor::SetupTimeline()
{
	FOnTimelineFloat ProgressFunction;
	ProgressFunction.BindUFunction(this, FName("HandleProgress"));

	FOnTimelineEvent TimelineFinished;
	TimelineFinished.BindUFunction(this, FName("OnTimelineFinished"));
	MyTimeline.SetTimelineFinishedFunc(TimelineFinished);

	MyTimeline.AddInterpFloat(FloatCurve, ProgressFunction);
	MyTimeline.SetLooping(false);
	MyTimeline.SetTimelineLengthMode(TL_TimelineLength);
	MyTimeline.SetTimelineLength(0.5f);
}

void APlantActor::HavestPlant()
{
	if (PlantState == EPlantState::Mature)
	{
		if (isRepeated)
		{

			PlantState = EPlantState::Havested;

			CurLevel = 1;

			NewMesh = LoadObject<UStaticMesh>(nullptr, *PlantInfo.HavestedPath1);
			if (NewMesh)
			{
				MeshComponent->SetStaticMesh(NewMesh);
				NewMesh = nullptr;
			}
		}
		else
		{
			Destroy();
		}
	}
}

void APlantActor::SpawnPaticleSystem(UParticleSystem* particle)
{
	if (particle)
	{
		ParticleComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), GrowParticleSystem, GetActorLocation(), GetActorRotation());
	}
}
