// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/Component/TalkComponent.h"

#include "Engine/OverlapResult.h"
#include "GameFramework/Character.h"
#include "JIU/PlantActor.h"
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
}

void UTalkComponent::BeginPlay()
{
	Super::BeginPlay();

	Player = CastChecked<ACharacter>(GetOwner());
	MyGameMode = CastChecked<AFarmLifeGameMode>(GetWorld()->GetAuthGameMode());
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
	// 이미 NPC와 대화중이라면 바로 통신
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
		return;
	}

	TArray<FOverlapResult> NPCResults;
	FCollisionQueryParams Params; 
	Params.AddIgnoredActor(Player);
	FVector Origin = Player->GetActorLocation();
	bool NPCOverlapped = GetWorld()->OverlapMultiByProfile(NPCResults, Origin, FQuat::Identity, TEXT("Pawn"), FCollisionShape::MakeSphere(300.0f), Params);
	DrawDebugSphere(GetWorld(), Origin, 300.0f, 16, FColor::Red, false, 2.0f);

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
			// TArray에 저장
			APlantActor* Plant = Cast<APlantActor>(Res.GetActor());
			if(Plant)
			{
				Plants.Add(Plant); UE_LOG(LogTemp, Log, TEXT("%s"), *Plant->GetName());
			}
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