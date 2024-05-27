// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/NPC/NPCBase.h"

#include "MediaPlayer.h"
#include "PKH/NPC/NPCController.h"
#include "MediaSoundComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Microsoft/AllowMicrosoftPlatformTypes.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "PKH/Animation/NPCAnimInstance.h"
#include "PKH/Game/FarmLifeGameMode.h"

ANPCBase::ANPCBase()
{
	PrimaryActorTick.bCanEverTick = false;

	// AI Controller
	static ConstructorHelpers::FClassFinder<ANPCController> NPCControllerClassRef(TEXT("/Game/PKH/Blueprint/BP_NPCController.BP_NPCController_C"));
	if(NPCControllerClassRef.Class)
	{
		AIControllerClass = NPCControllerClassRef.Class;
		AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	}

	GetCharacterMovement()->MaxWalkSpeed = 100.0f;

	// Animation
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimRef(TEXT("/Game/PKH/Anim/ABP_NPC.ABP_NPC_C"));
	if (AnimRef.Class)
	{
		GetMesh()->SetAnimClass(AnimRef.Class);
	}

	GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));

	// Media Sound Component
	MediaComp = CreateDefaultSubobject<UMediaSoundComponent>(TEXT("MediaComp"));

	NPCNameMap.Add(UEnum::GetValueAsString(ENPCType::Mira), TEXT("미라"));
	NPCNameMap.Add(UEnum::GetValueAsString(ENPCType::Junho), TEXT("이준호"));
	NPCNameMap.Add(UEnum::GetValueAsString(ENPCType::Chunsik), TEXT("이춘식"));
}

void ANPCBase::BeginPlay()
{
	Super::BeginPlay();

	NPCController = CastChecked<ANPCController>(GetController());
	MyGameMode = CastChecked<AFarmLifeGameMode>(GetWorld()->GetAuthGameMode());

	AnimInstance = Cast<UNPCAnimInstance>(GetMesh()->GetAnimInstance());
	ensure(AnimInstance);

	NPCName = NPCNameMap[UEnum::GetValueAsString(NPCType)];

	// Media Player Initialize
	MediaPlayer = NewObject<UMediaPlayer>();
	MediaPlayer->OnEndReached.AddDynamic(this, &ANPCBase::OnPlayEnded);

	const FString DefaultPath = UKismetSystemLibrary::GetProjectDirectory() + TEXT("Extras/WavFiles/Default.wav");
	PlayTTS(DefaultPath);
}

void ANPCBase::StartConversation()
{
	if(ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
	{
		// Turn to player
		FVector DirectionVec = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		DirectionVec.Z = 0;
		const FRotator TargetRot= DirectionVec.ToOrientationRotator();
		SetActorRotation(TargetRot);

		NPCController->StartConversation();
		AnimInstance->PlayMontage_Conv();
	}
}

void ANPCBase::EndConversation()
{
	NPCController->EndConversation();
	AnimInstance->StopAllMontages(0);
}

#pragma region TTS
void ANPCBase::SetCurEmotion(const FString& NewEmotion)
{
	CurEmotion = NewEmotion;
}

void ANPCBase::PlayEmotion()
{
	if(false == CurEmotion.IsEmpty())
	{
		AnimInstance->PlayMontage_Emotion(CurEmotion);
	}
}

void ANPCBase::PlayTTS(const FString& FilePath)
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
	if(false == MediaPlayer->IsClosed())
	{
		MediaPlayer->Close();
	}
}
#pragma endregion


#pragma region Talk To Player
void ANPCBase::RequestTTS()
{
	MyGameMode->RequestTTS(this, GreetingText);
	IsRequestingTTS = true;
}

void ANPCBase::SetTTSPath(const FString& NewTTSPath)
{
	NPCTTSPath = NewTTSPath;
	IsRequestingTTS = false;
}

void ANPCBase::TalkToPlayer()
{
	StartConversation();
	MyGameMode->TalkToPlayer(GreetingText, GreetingEmotion);
	if(false == IsRequestingTTS)
	{
		PlayTTS(NPCTTSPath);
	}
}
#pragma endregion


#pragma region 호감도
void ANPCBase::UpdateLikeability(int32 InLikeability)
{
	CurLikeability = FMath::Clamp(CurLikeability + InLikeability, 0, MaxLikeability);

	if(OnLikeabilityChanged.IsBound())
	{
		OnLikeabilityChanged.Broadcast();
	}
}

bool ANPCBase::IsMaxLikeability()
{
	return CurLikeability == MaxLikeability;
}

void ANPCBase::GivePresent(int32 NewItemId)
{
	UpdateLikeability(NewItemId == PreferItemId ? PreferItemValue : NormalItemValue);

	// 통신
	AFarmLifeGameMode* GameMode = CastChecked<AFarmLifeGameMode>(GetWorld()->GetAuthGameMode());
	GameMode->SendText(TEXT("이거 선물이야, 받아줘!"), this);
}
#pragma endregion