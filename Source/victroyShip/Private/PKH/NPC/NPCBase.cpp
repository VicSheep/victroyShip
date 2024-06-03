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
	PrimaryActorTick.bCanEverTick = true;

	// Emotion UI
	EmotionUIComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("EmotionUIComp"));
	EmotionUIComp->SetupAttachment(RootComponent);
	EmotionUIComp->SetRelativeLocation(FVector(0, 35, 120));
	EmotionUIComp->SetRelativeScale3D(FVector(0.6f));
	EmotionUIComp->SetDrawSize(FVector2D(400, 400));

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

	NPCNameMap.Add(UEnum::GetValueAsString(ENPCType::Mira), Name_Mira);
	NPCNameMap.Add(UEnum::GetValueAsString(ENPCType::Junho), Name_Junho);
	NPCNameMap.Add(UEnum::GetValueAsString(ENPCType::Chunsik), Name_Chunsik);
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

	NPCName = NPCNameMap[UEnum::GetValueAsString(NPCType)];

	// Media Player Initialize
	MediaPlayer = NewObject<UMediaPlayer>();
	MediaPlayer->OnEndReached.AddDynamic(this, &ANPCBase::OnPlayEnded);

	if(CurLikeability >= FriendlyLikeability)
	{
		InitGreeting();
	}

	const FString DefaultPath = UKismetSystemLibrary::GetProjectDirectory() + TEXT("Extras/WavFiles/Default.wav");
	PlayTTS(DefaultPath);
}

void ANPCBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	const FVector CameraLoc = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraLocation();
	const FRotator UIRotation = (CameraLoc - GetActorLocation()).GetSafeNormal().ToOrientationRotator();
	EmotionUIComp->SetWorldRotation(UIRotation);
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
	EmotionUI->SetVisibility(ESlateVisibility::Hidden);
	NPCController->EndConversation();
	AnimInstance->StopAllMontages(0);
}

#pragma region TTS
void ANPCBase::SetCurEmotion(const FString& NewEmotion)
{
	CurEmotion = NewEmotion;
}

void ANPCBase::SetCurEmotion(EEmotion NewEmotion)
{
	CurEmotion = UEnum::GetValueAsString(NewEmotion).Mid(10);
}

void ANPCBase::PlayEmotion()
{
	if(false == CurEmotion.IsEmpty())
	{
		AnimInstance->PlayMontage_Emotion(CurEmotion);
		EmotionUI->SetEmotion(CurEmotion);
		EmotionUI->SetVisibility(ESlateVisibility::Visible);
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

void ANPCBase::GivePresent(int32 NewItemId)
{
	UpdateLikeability(NewItemId == PreferItemId ? PreferItemValue : NormalItemValue);

	// 통신
	AFarmLifeGameMode* GameMode = CastChecked<AFarmLifeGameMode>(GetWorld()->GetAuthGameMode());
	GameMode->SendText(PresentText, this);
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
