// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/Component/TalkComponent.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/Character.h"
#include "JIU/PlantActor.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PKH/Game/FarmLifeGameMode.h"
#include "PKH/NPC/NPCBase.h"

// Sets default values for this component's properties
UTalkComponent::UTalkComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Speech File
	RecordFileDir = UKismetSystemLibrary::GetProjectDirectory() + TEXT("Extras/WavFiles/");
	RecordFilePath = RecordFileDir + RecordFileName + TEXT(".wav");

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> Vfx_TalkRef(TEXT("/Script/Niagara.NiagaraSystem'/Game/PKH/NS/Vfx_Talk.Vfx_Talk'"));
	if(Vfx_TalkRef.Object)
	{
		Vfx_Talk = Vfx_TalkRef.Object;
	}
}

void UTalkComponent::BeginPlay()
{
	Super::BeginPlay();

	Player = CastChecked<ACharacter>(GetOwner());
	MyGameMode = CastChecked<AFarmLifeGameMode>(GetWorld()->GetAuthGameMode());

	UActorComponent* NiagaraComp = Player->GetComponentByClass(UNiagaraComponent::StaticClass());
	if(NiagaraComp)
	{
		VfxComp = Cast<UNiagaraComponent>(NiagaraComp);
		if(VfxComp)
		{
			VfxComp->SetAsset(Vfx_Talk);
			VfxComp->SetAutoDestroy(false);
			VfxComp->SetAutoActivate(false);
		}
	}
}

#pragma region Check Nearby
void UTalkComponent::CheckNearbyObjects()
{
	const FString Input = TEXT("");
	SearchNearby(Input);
}

void UTalkComponent::CheckNearbyObjects(const FString& InputText)
{
	SearchNearby(InputText);
}

void UTalkComponent::SearchNearby(const FString& InputText)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	ANPCBase* CurNPC = MyGameMode->GetCurNPC();
	if(CurNPC)
	{
		if (InputText.IsEmpty())
		{
			ConversationWithNPC(CurNPC);
		}
		else
		{
			ConversationWithNPCByText(CurNPC, InputText);
		}
		
		MyGameMode->ChangeInputMode_Both();
		return;
	}

	TArray<FOverlapResult> NPCResults;
	FCollisionQueryParams Params; 
	Params.AddIgnoredActor(Player);
	FVector Origin = Player->GetActorLocation();
	bool NPCOverlapped = GetWorld()->OverlapMultiByProfile(NPCResults, Origin, FQuat::Identity, TEXT("Pawn"), FCollisionShape::MakeSphere(300.0f), Params);

	if (NPCOverlapped)
	{
		ANPCBase* TargetNPC = nullptr;
		float MinDistance = 500.0f;
		for (FOverlapResult& Res : NPCResults)
		{
			ANPCBase* NPC = Cast<ANPCBase>(Res.GetActor());
			if (nullptr == NPC)
			{
				continue;
			}

			const float CurDistance = FVector::Dist(Origin, NPC->GetActorLocation());
			if (MinDistance > CurDistance)
			{
				TargetNPC = NPC;
				MinDistance = CurDistance;
			}
		}

		if (nullptr != TargetNPC)
		{
			if(InputText.IsEmpty())
			{
				ConversationWithNPC(TargetNPC);
			}
			else
			{
				ConversationWithNPCByText(TargetNPC, InputText);
			}
			
			MyGameMode->ChangeInputMode_Both();
			return;
		}
	}

	TArray<FOverlapResult> PlantResults;
	bool PlantOverlapped = GetWorld()->OverlapMultiByChannel(PlantResults, Origin, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(300.0f), Params);

	if (PlantOverlapped)
	{
		TArray<TObjectPtr<APlantActor>> Plants;
		for (FOverlapResult& Res : PlantResults)
		{
			// TArray
			APlantActor* Plant = Cast<APlantActor>(Res.GetActor());
			if(Plant)
			{
				Plants.Add(Plant); UE_LOG(LogTemp, Log, TEXT("%s"), *Plant->GetName());
			}
		}

		if(Plants.IsEmpty())
		{
			return;
		}

		if(InputText.IsEmpty())
		{
			TalkToPlant(Plants);
		}
		else
		{
			TalkToPlantByText(Plants, InputText);
		}
	}
}
#pragma endregion

#pragma region Communication
void UTalkComponent::ConversationWithNPC(ANPCBase* NewNPC)
{
	InConversation = true;
	MyGameMode->SendSpeech(RecordFileName, RecordFilePath, NewNPC);
}

void UTalkComponent::ConversationWithNPCByText(ANPCBase* NewNPC, const FString& InputText)
{
	InConversation = true;
	MyGameMode->SendText(InputText, NewNPC);
}

void UTalkComponent::TalkToPlant(const TArray<TObjectPtr<APlantActor>>& NewPlants)
{
	InConversation = true;
	MyGameMode->TalkToPlant(RecordFileName, RecordFilePath, NewPlants);
}

void UTalkComponent::TalkToPlantByText(const TArray<TObjectPtr<APlantActor>>& NewPlants, const FString& InputText)
{
	InConversation = true;
	MyGameMode->TalkToPlantWithText(InputText, NewPlants);
}
#pragma endregion

#pragma region Record On/Off
void UTalkComponent::RecordOn()
{
	MyGameMode->RecordOn();
	TalkRangeOn();

	IsRecordValid = false;
	GetWorld()->GetTimerManager().SetTimer(RecordTimeHandle, FTimerDelegate::CreateLambda([this]()
	{
		IsRecordValid = true;;
	}), ValidRecordTime, false);
}

void UTalkComponent::RecordOff()
{
	MyGameMode->RecordOff();
	TalkRangeOff();

	FTimerManager& Manager = GetWorld()->GetTimerManager();
	if(Manager.IsTimerActive(RecordTimeHandle))
	{
		Manager.ClearTimer(RecordTimeHandle);
	}
}

void UTalkComponent::TalkRangeOn()
{
	if (nullptr == VfxComp)
	{
		return;
	}

	VfxComp->Activate();
}

void UTalkComponent::TalkRangeOff()
{
	VfxComp->Deactivate();
}

bool UTalkComponent::IsValidRecordTime()
{
	return IsRecordValid;
}
#pragma endregion