// Fill out your copyright notice in the Description page of Project Settings.


#include "JIU/GroundActor.h"

#include "TimerManager.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "JIU/PlantActor.h"
#include "JIU/WeedActor.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/SpringArmComponent.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
AGroundActor::AGroundActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	SetRootComponent(BoxComponent); // Set Component on Root Component
	BoxComponent->SetBoxExtent(FVector(180.f, 180.f, 180.f));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent); // Attach to Root Component
	MeshComponent->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, *PlanterPath));

	ActorComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("ActorComponent"));
	ActorComponent->SetupAttachment(RootComponent);
	ActorComponent->SetChildActorClass(AWeedActor::StaticClass());

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 360.f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SetRelativeRotation(FRotator(-15.f, 0.f, 0.f));

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(CameraBoom);
	CameraComponent->bUsePawnControlRotation = false;
	// CameraComponent->SetRelativeLocationAndRotation(FVector(-360.f, 0.f, 180.f), FRotator(-15.f, 0.f, 0.f));

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

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MiddleMaterialAsset(*MiddleMaterialPath);
	if (MiddleMaterialAsset.Succeeded())
	{
		MiddleMaterialInterface = MiddleMaterialAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> WetMaterialAsset(*WetMaterialPath);
	if (WetMaterialAsset.Succeeded())
	{
		WetMaterialInterface = WetMaterialAsset.Object;
	}

	DustNiagaraSystem = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/JIU/Effects/NS_Explosion_Sand.NS_Explosion_Sand"));
	RainNiagaraSystem = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/JIU/Effects/NS_Environment_Rain_Custom.NS_Environment_Rain_Custom"));
	LeafNiagaraSystem = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/Realistic_Starter_VFX_Pack_Niagara/Niagara/Environment/NS_Environment_Leaves_Vortex.NS_Environment_Leaves_Vortex"));
}

// Called when the game starts or when spawned
void AGroundActor::BeginPlay()
{
	Super::BeginPlay();

	PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		PP = PC->GetPawn();

		if (PP)
		{
			UActorComponent* comp = PP->GetComponentByClass(UPrimitiveComponent::StaticClass());
			if (comp)
			{
				PrimitiveComponent = Cast<UPrimitiveComponent>(comp);
			}
		}
	}

	if (DefaultMaterialInterface)
	{
		MeshComponent->SetMaterial(0, DefaultMaterialInterface);
		SetGroundState(EGroundState::Default);
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

		if (Plant)
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
			if (Cushion < 10)
			{
				Cushion += 1;
				return;
			}

			RandomNumber = FMath::RandRange(1, 100);
			// UE_LOG(LogTemp, Warning, TEXT("%d"), RandomNumber);

			if (GroundState == EGroundState::DryPlanter)
			{
				if (RandomNumber <= 20)
				{
					Cushion = 0;
					SetGroundState(EGroundState::Default);
					MeshComponent->SetMaterial(0, DefaultMaterialInterface);
				}
			}
			else if (GroundState == EGroundState::Default && !isWeed)
			{
				if (RandomNumber <= 20)
				{
					Cushion = 0;
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
		Plant = GetWorld()->SpawnActor<APlantActor>(PlantFactory, BoxComponent->GetComponentLocation() + FVector(0.f, 0.f, 20.f), FRotator(0.f));
		Plant->SetPlant(id, this);
	}
}

void AGroundActor::WaterPlant()
{
	if (GroundState != EGroundState::Default)
	{
		WaterFigure = 100.f;
		SpawnNiagaraSystem(RainNiagaraSystem);
		SetGroundMaterial();
	}
}

void AGroundActor::FertilizePlant()
{
	if (GroundState != EGroundState::Default)
	{
		FertilizerFigure = 100.f;
		SpawnNiagaraSystem(DustNiagaraSystem);
		SetGroundMaterial();
	}
}

void AGroundActor::RemovePlant()
{
	haveChange = true;

	if (Plant)
	{
		Plant->Destroy();
		Plant = nullptr;

		SpawnNiagaraSystem(LeafNiagaraSystem);
	}
}

void AGroundActor::ProwGround()
{
	if (!isWeed)
	{
		if (GroundState == EGroundState::Default)
		{
			SetGroundState(EGroundState::DryPlanter);
			Cushion = 0;

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
		Cushion = 0;
	}
}

void AGroundActor::MoveCamera()
{
	if (PC && PP)
	{
		// 카메라 전환을 부드럽게 하기 위한 블렌드 시간과 블렌드 함수 설정
		float BlendTime = 0.5f;
		EViewTargetBlendFunction BlendFunc = VTBlend_Cubic;
		float BlendExp = 0.0f;
		bool bLockOutgoing = false;

		// 카메라 전환
		if (!isZoom)
		{
			/*FVector MyLocation = this->GetActorLocation();
			FVector TargetLocation = PP->GetActorLocation();
			FVector DirectionToTarget = TargetLocation - MyLocation;

			DirectionToTarget.Z = 0.0f;
			DirectionToTarget = DirectionToTarget.GetSafeNormal();
			FVector MyForward = GetActorForwardVector().GetSafeNormal();

			float DotProduct = FVector::DotProduct(MyForward, DirectionToTarget);
			float AngleRadians = FMath::Acos(DotProduct);

			FVector CrossProduct = FVector::CrossProduct(MyForward, DirectionToTarget);
			if (CrossProduct.Z < 0)
			{
				AngleRadians = -AngleRadians;
			}

			float SineOfAngle = FMath::Sin(AngleRadians);
			float CosineOfAngle = FMath::Cos(AngleRadians);

			float distance = 360.f;
			CameraComponent->SetRelativeLocation(FVector(CosineOfAngle * distance, SineOfAngle * distance, 180.f));

			DirectionToTarget = FVector(0.f, 0.f, 0.f) - FVector(CosineOfAngle * distance, SineOfAngle * distance, 180.f).GetSafeNormal();
			FRotator LookAtRotation = UKismetMathLibrary::MakeRotFromX(DirectionToTarget);
			LookAtRotation.Pitch = -15.f;
			CameraComponent->SetRelativeRotation(LookAtRotation);

			PP->SetActorRotation(LookAtRotation);*/

			PC->SetViewTargetWithBlend(this, BlendTime, BlendFunc, BlendExp, bLockOutgoing);

			if (PrimitiveComponent)
			{
				PrimitiveComponent->SetVisibility(false);
			}

			isZoom = true;
		}
		else
		{
			PC->SetViewTargetWithBlend(PP, BlendTime, BlendFunc, BlendExp, bLockOutgoing);

			if (PrimitiveComponent)
			{
				PrimitiveComponent->SetVisibility(true);
			}

			isZoom = false;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Player Controller"));
	}
}

void AGroundActor::SetGroundMaterial()
{
	if (GroundState == EGroundState::DryPlanter)
	{
		if (WaterFigure > figureLimit || FertilizerFigure > figureLimit)
		{
			if (WaterFigure > figureLimit && FertilizerFigure > figureLimit)
			{
				if (WetMaterialInterface)
				{
					MeshComponent->SetMaterial(0, WetMaterialInterface);
					SetGroundState(EGroundState::WetPlanter);
				}
			}
			else
			{
				if (MiddleMaterialInterface)
				{
					MeshComponent->SetMaterial(0, MiddleMaterialInterface);
				}
			}
		}
	}

	else if (GroundState == EGroundState::WetPlanter)
	{
		if (WaterFigure < figureLimit || FertilizerFigure < figureLimit)
		{
			if (WaterFigure < figureLimit && FertilizerFigure < figureLimit)
			{
				if (DryMaterialInterface)
				{
					MeshComponent->SetMaterial(0, DryMaterialInterface);
					SetGroundState(EGroundState::DryPlanter);
				}
			}
			else
			{
				if (MiddleMaterialInterface)
				{
					MeshComponent->SetMaterial(0, MiddleMaterialInterface);
					SetGroundState(EGroundState::DryPlanter);
				}
			}
		}
	}
}

void AGroundActor::SetGroundState(EGroundState state)
{
	/*
	if (state == EGroundState::Default && state == EGroundState::DryPlanter)
	{
		Cushion = 0;
	}
	*/

	GroundState = state;
}

void AGroundActor::SpawnNiagaraSystem(UNiagaraSystem* niagara)
{
	if (niagara)
	{
		NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), niagara, GetActorLocation(), GetActorRotation());
	}
}
