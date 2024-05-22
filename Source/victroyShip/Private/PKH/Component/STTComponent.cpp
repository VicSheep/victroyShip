// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/Component/STTComponent.h"

#include "MediaPlayer.h"
#include "MediaSoundComponent.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/Character.h"
#include "JIU/PlantActor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PKH/Game/FarmLifeGameMode.h"
#include "PKH/NPC/NPCBase.h"

// Sets default values for this component's properties
USTTComponent::USTTComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Speech File
	RecordFileDir = UKismetSystemLibrary::GetProjectDirectory() + TEXT("Saved/BouncedWavFiles/");
	RecordFilePath = RecordFileDir + RecordFileName + TEXT(".wav");

	DefaultFilePath = TEXT("D:/Projects/victroyShip/Saved/BouncedWavFiles/Default.wav");
}

void USTTComponent::BeginPlay()
{
	Super::BeginPlay();

	Player = CastChecked<ACharacter>(GetOwner());
	MyGameMode = CastChecked<AFarmLifeGameMode>(GetWorld()->GetAuthGameMode());

	MediaComp = Cast<UMediaSoundComponent>(Player->GetComponentByClass(UMediaSoundComponent::StaticClass()));
	MediaPlayer = NewObject<UMediaPlayer>();
	MediaPlayer->OnEndReached.AddDynamic(this, &USTTComponent::OnPlayEnded);

	if(MediaComp)
	{
		if (MediaPlayer->OpenFile(DefaultFilePath))
		{
			MediaComp->SetMediaPlayer(MediaPlayer);
			UE_LOG(LogTemp, Warning, TEXT("[STTComponent] Initialize Complete"));
		}
	}
}

#pragma region Check Nearby
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
	bool NPCOverlapped = GetWorld()->OverlapMultiByProfile(NPCResults, Origin, FQuat::Identity, TEXT("Pawn"),
															  FCollisionShape::MakeSphere(300.0f), Params);
	DrawDebugSphere(GetWorld(), Origin, 300.0f, 16, FColor::Red, false, 2.0f);

	if (NPCOverlapped)
	{
		ANPCBase* TargetNPC = nullptr;
		float MinDistance = 500.0f;
		for (FOverlapResult& Res : NPCResults)
		{
			UE_LOG(LogTemp, Log, TEXT("%s"), *Res.GetActor()->GetName());
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
	bool PlantOverlapped = GetWorld()->OverlapMultiByProfile(PlantResults, Origin, FQuat::Identity, TEXT("OverlapAllDynamic"),
		FCollisionShape::MakeSphere(300.0f), Params);

	if (PlantOverlapped)
	{
		TArray<TObjectPtr<APlantActor>> Plants;
		for (FOverlapResult& Res : PlantResults)
		{
			// TArray에 저장
			APlantActor* Plant = Cast<APlantActor>(Res.GetActor());
			if(Plant)
			{
				Plants.Add(Plant);
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

#pragma region Voice
void USTTComponent::PlayVoice(const FString& FilePath)
{
	if(nullptr == MediaComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("No MediaSoundComponent"));
		return;
	}

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([this, FilePath]()
	{
		if (MediaPlayer->OpenFile(FilePath))
		{
			MediaComp->SetMediaPlayer(MediaPlayer);
			UE_LOG(LogTemp, Warning, TEXT("Open File Success : %s"), *FilePath);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Open File Failed : %s"), *FilePath);
		}
	}), 0.3f, false);
}

void USTTComponent::OnPlayEnded()
{
	if(false == MediaPlayer->IsClosed())
	{
		MediaPlayer->Close();
		MyGameMode->StartConversation();
	}
}
#pragma endregion

#pragma region Communication
void USTTComponent::ConversationWithNPC(ANPCBase* NewNPC)
{
	PlayVoice(RecordFilePath);
	MyGameMode->SendSpeech(RecordFileName, RecordFilePath, NewNPC);
}

void USTTComponent::ConversationWithNPCByText(ANPCBase* NewNPC, const FString& InputText)
{
	MyGameMode->SendText(InputText, NewNPC);
}

void USTTComponent::TalkToPlant(const TArray<TObjectPtr<APlantActor>>& NewPlants)
{
	PlayVoice(RecordFilePath);
	MyGameMode->TalkToPlant(RecordFileName, RecordFilePath, NewPlants);
}

void USTTComponent::TalkToPlantByText(const TArray<TObjectPtr<APlantActor>>& NewPlants, const FString& InputText)
{
	MyGameMode->TalkToPlantWithText(InputText, NewPlants);
}
#pragma endregion
