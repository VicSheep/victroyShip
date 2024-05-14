// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/NPC/NPCBase.h"

#include "MediaPlayer.h"
#include "PKH/NPC/NPCController.h"
#include "MediaSoundComponent.h"

ANPCBase::ANPCBase()
{
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = ANPCController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// Mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshRef(TEXT(""));
	if(MeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(MeshRef.Object);
	}

	// Animation
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimRef(TEXT(""));
	if (AnimRef.Class)
	{
		GetMesh()->SetAnimClass(AnimRef.Class);
	}

	GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));

	// Media Sound Component
	MediaComp = CreateDefaultSubobject<UMediaSoundComponent>(TEXT("MediaComp"));
}

void ANPCBase::BeginPlay()
{
	Super::BeginPlay();

	MediaPlayer = NewObject<UMediaPlayer>();
	MediaPlayer->OnEndReached.AddDynamic(this, &ANPCBase::OnPlayEnded);

	const FString& FilePath = TEXT("D:/Projects/victroyShip/Saved/BouncedWavFiles/Speech.wav");
	LoadSpeechFileAndPlay(FilePath);
}

void ANPCBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

#pragma region TTS
void ANPCBase::LoadSpeechFileAndPlay(const FString& FilePath)
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
}

void ANPCBase::OnPlayEnded()
{
	MediaPlayer->Close();
}
#pragma endregion

#pragma region 호감도
void ANPCBase::LikeabilityUp(int32 InLikeability)
{
	CurLikeability = FMath::Clamp(CurLikeability + InLikeability, 0, MaxLikeability);
}

bool ANPCBase::IsMaxLikeability()
{
	return CurLikeability == MaxLikeability;
}
#pragma endregion
