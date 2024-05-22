// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/Component/STTComponent.h"

#include "Engine/OverlapResult.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PKH/Game/FarmLifeGameMode.h"
#include "PKH/NPC/NPCBase.h"

// Sets default values for this component's properties
USTTComponent::USTTComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Speech File
	SpeechFileDir = UKismetSystemLibrary::GetProjectDirectory() + TEXT("Saved/BouncedWavFiles/");
}

void USTTComponent::BeginPlay()
{
	Super::BeginPlay();

	Player = Cast<ACharacter>(GetOwner());
	MyGameMode = CastChecked<AFarmLifeGameMode>(GetWorld()->GetAuthGameMode());
}

void USTTComponent::CheckNearbyObjects()
{
	const FString Input = TEXT("");
	SearchNearby(Input);
}

void USTTComponent::CheckNearbyObjects(const FString& InputText)
{
	SearchNearby(InputText);
}

void USTTComponent::SearchNearby(const FString& InputText)
{
	TArray<FOverlapResult> NPCResults;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Player);
	FVector Origin = Player->GetActorLocation();
	bool NPCOverlapped = GetWorld()->OverlapMultiByProfile(NPCResults, Origin, FQuat::Identity, TEXT("Pawn"),
		FCollisionShape::MakeSphere(300.0f), Params);
	DrawDebugSphere(GetWorld(), Origin, 300.0f, 16, FColor::Red, false, 2.0f); UE_LOG(LogTemp, Log, TEXT("NPC Overlap : %d"), NPCOverlapped);

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
				ConversationWithNPC(TargetNPC);
			}
		}
		return;
	}

	TArray<FOverlapResult> PlantResults;
	bool PlantOverlapped = GetWorld()->OverlapMultiByProfile(PlantResults, Origin, FQuat::Identity, TEXT("SearchPlant"),
		FCollisionShape::MakeSphere(300.0f), Params);

	if (PlantOverlapped)
	{
		TArray<TObjectPtr<AActor>> Plants;
		for (FOverlapResult& Res : PlantResults)
		{
			// TArray�� ���� 
			Plants.Add(Res.GetActor());
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

void USTTComponent::ConversationWithNPC(ANPCBase* NewNPC)
{
	const FString& FullPath = SpeechFileDir + SpeechFileName + TEXT(".wav");
	MyGameMode->SendSpeech(SpeechFileName, FullPath, NewNPC);
}

void USTTComponent::ConversationWithNPCByText(ANPCBase* NewNPC, const FString& InputText)
{
	MyGameMode->SendText(InputText, NewNPC);
}

void USTTComponent::TalkToPlant(const TArray<TObjectPtr<AActor>>& NewPlants)
{
	const FString& FullPath = SpeechFileDir + SpeechFileName + TEXT(".wav");
	MyGameMode->TalkToPlant(SpeechFileName, FullPath, NewPlants);
}

void USTTComponent::TalkToPlantByText(const TArray<TObjectPtr<AActor>>& NewPlants, const FString& InputText)
{
	//MyGameMode->SendSpeech();
}
