// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/NPC/NPCBase.h"

#include "MediaPlayer.h"
#include "PKH/NPC/NPCController.h"
#include "MediaSoundComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Microsoft/AllowMicrosoftPlatformTypes.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "OJS/Player/FarmLifeOjsPlayerCharacter.h"
#include "PKH/Animation/NPCAnimInstance.h"
#include "PKH/Component/TalkComponent.h"
#include "PKH/Game/FarmLifeGameMode.h"
#include "PKH/UI/EmotionUIWidget.h"

ANPCBase::ANPCBase()
{
	PrimaryActorTick.bCanEverTick = false;

	// Emotion UI
	EmotionUIComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("EmotionUIComp"));
	EmotionUIComp->SetupAttachment(RootComponent);
	EmotionUIComp->SetRelativeLocation(FVector(0, 35, 120));
	EmotionUIComp->SetRelativeScale3D(FVector(0.6f));
	EmotionUIComp->SetDrawSize(FVector2D(400, 400));
	EmotionUIComp->SetWidgetSpace(EWidgetSpace::Screen);

	static ConstructorHelpers::FClassFinder<UEmotionUIWidget> EmotionUIClassRef(TEXT("/Game/PKH/UI/WBP_EmotionUI.WBP_EmotionUI_C"));
	if (EmotionUIClassRef.Class)
	{
		EmotionUIComp->SetWidgetClass(EmotionUIClassRef.Class);
	}

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

	NPCNameMap.Add(ENPCType::Cafe, Name_Minwoo);
	NPCNameMap.Add(ENPCType::Artist, Name_Ayeong);
	NPCNameMap.Add(ENPCType::Programmer, Name_Jimin);
	NPCNameMap.Add(ENPCType::Unemployed, Name_Junho);
	NPCNameMap.Add(ENPCType::Farmer, Name_Chawon);
	NPCNameMap.Add(ENPCType::Fisherman, Name_Chunsik);
}

void ANPCBase::BeginPlay()
{
	Super::BeginPlay();

	MyGameMode = CastChecked<AFarmLifeGameMode>(GetWorld()->GetAuthGameMode());

	NPCController = CastChecked<ANPCController>(GetController());
	NPCController->SetHomeLoc(HomeLoc);

	AnimInstance = Cast<UNPCAnimInstance>(GetMesh()->GetAnimInstance());
	ensure(AnimInstance);

	EmotionUI = Cast<UEmotionUIWidget>(EmotionUIComp->GetWidget());
	ensure(EmotionUI);
	EmotionUI->SetVisibility(ESlateVisibility::Hidden);

	NPCName = NPCNameMap[NPCType];

	// Media Player Initialize
	MediaPlayer = NewObject<UMediaPlayer>();
	MediaPlayer->OnEndReached.AddDynamic(this, &ANPCBase::OnPlayEnded);

	// Init Greeting & Present Data
	if(CurLikeability >= FriendlyLikeability)
	{
		InitGreeting();
	}

	const FString DefaultPath = UKismetSystemLibrary::GetProjectDirectory() + TEXT("Extras/WavFiles/Default.wav");
	PlayTTS(DefaultPath);
}

#pragma region Start / End Conversation
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
		EmotionUI->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ANPCBase::EndConversation()
{
	EmotionUI->SetVisibility(ESlateVisibility::Hidden);
	NPCController->EndConversation();
	AnimInstance->StopAllMontages(0);
	SetNPCWalk();
}
#pragma endregion


#pragma region TTS
void ANPCBase::SetCurEmotion(const FString& NewEmotion)
{
	CurEmotion = NewEmotion;
}

void ANPCBase::SetCurEmotion(EEmotion NewEmotion)
{
	CurEmotion = UEnum::GetValueAsString(NewEmotion).Mid(10);
}

void ANPCBase::PlayEmotion(bool IsUIOnly)
{
	if(CurEmotion.IsEmpty())
	{
		return;
	}

	EmotionUI->SetEmotion(CurEmotion);
	EmotionUI->SetVisibility(ESlateVisibility::Visible);
	if(false == IsUIOnly)
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


#pragma region Speed
void ANPCBase::SetNPCWalk()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void ANPCBase::SetNPCRun()
{
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}
#pragma endregion


#pragma region Greeting
void ANPCBase::InitGreeting()
{
	MyGameMode->InitGreeting(NPCName, CurLikeability);
	HasIntendToGreeting = true;
}

void ANPCBase::GreetingToPlayer()
{
	// Set Blackboard Key & Player State
	AFarmLifeOjsPlayerCharacter* Player = Cast<AFarmLifeOjsPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if(nullptr == Player)
	{
		UE_LOG(LogTemp, Error, TEXT("[GreetingPlayer] There is no player"));
		return;
	}
	UTalkComponent* TalkComp = Cast<UTalkComponent>(Player->GetComponentByClass(UTalkComponent::StaticClass()));
	if (nullptr == TalkComp)
	{
		UE_LOG(LogTemp, Error, TEXT("[GreetingPlayer] There is no TalkComponent"));
		return;
	}
	TalkComp->StartConversation();
	StartConversation();

	MyGameMode->RequestGreetingData(this);
	HasIntendToGreeting = false;

	SetCurEmotion(EEmotion::joy);
	PlayEmotion();
}

bool ANPCBase::IsFriendly() const
{
	return (CurLikeability >= FriendlyLikeability) && HasIntendToGreeting;
}

void ANPCBase::SetEmotionUI(bool IsActive)
{
	EmotionUI->SetVisibility(IsActive ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
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
#pragma endregion


#pragma region Present
void ANPCBase::GivePresent(const FString& ItemName)
{
	/*if (GetPresentToday)
	{
		return;
	}

	GetPresentToday = true;*/
	bool bIsPrefer = ItemName == PreferItemName;
	UpdateLikeability(bIsPrefer ? PreferItemValue : NormalItemValue);

	// 통신
	AFarmLifeGameMode* GameMode = CastChecked<AFarmLifeGameMode>(GetWorld()->GetAuthGameMode());
	GameMode->RequestPresentData(this, bIsPrefer);
}
#pragma endregion

void ANPCBase::DoJob()
{
	UE_LOG(LogTemp, Warning, TEXT("[%s] Do Job"), *NPCName);

	SetActorRotation(WorkRotation);
	AnimInstance->PlayMontage_Custom(Montage_Work);
}

void ANPCBase::OnDateUpdated(int32 NewDate)
{
	if(CurLikeability >= FriendlyLikeability)
	{
		InitGreeting();
	}

	SetActorLocation(HomeLoc);
}
