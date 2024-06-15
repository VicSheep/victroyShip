// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/NPC/NPCBase.h"

#include "MediaPlayer.h"
#include "PKH/NPC/NPCController.h"
#include "MediaSoundComponent.h"
#include "NiagaraComponent.h"
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
#include "NiagaraFunctionLibrary.h"

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

	// Sound
	SfxComp = CreateDefaultSubobject<UAudioComponent>(TEXT("SfxComp"));
	SfxComp->SetVolumeMultiplier(0.6f);

	static ConstructorHelpers::FObjectFinder<USoundBase> Sfx_NoticeRef(TEXT("/Script/Engine.SoundWave'/Game/PKH/Sound/Sfx_Notice.Sfx_Notice'"));
	if (Sfx_NoticeRef.Object)
	{
		Sfx_Notice = Sfx_NoticeRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> Sfx_JoyRef(TEXT("/Script/Engine.SoundWave'/Game/PKH/Sound/Sfx_Joy.Sfx_Joy'"));
	if (Sfx_JoyRef.Object)
	{
		Sfx_Joy = Sfx_JoyRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> Sfx_AngerRef(TEXT("/Script/Engine.SoundWave'/Game/PKH/Sound/Sfx_Angry.Sfx_Angry'"));
	if (Sfx_AngerRef.Object)
	{
		Sfx_Anger = Sfx_AngerRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> Sfx_SadRef(TEXT("/Script/Engine.SoundWave'/Game/PKH/Sound/Sfx_Sad.Sfx_Sad'"));
	if (Sfx_SadRef.Object)
	{
		Sfx_Sad = Sfx_SadRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> Sfx_SurpriseRef(TEXT("/Script/Engine.SoundWave'/Game/PKH/Sound/Sfx_Surprised.Sfx_Surprised'"));
	if (Sfx_SurpriseRef.Object)
	{
		Sfx_Surprise = Sfx_SurpriseRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> Sfx_IndiffRef(TEXT("/Script/Engine.SoundWave'/Game/PKH/Sound/Sfx_Indiff.Sfx_Indiff'"));
	if (Sfx_IndiffRef.Object)
	{
		Sfx_Indiff = Sfx_IndiffRef.Object;
	}

	// Vfx
	VfxComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("VfxComp"));
	VfxComp->SetupAttachment(RootComponent);
	VfxComp->AddRelativeLocation(FVector(0, 0, 20));
	VfxComp->bAutoActivate = false;

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> Vfx_LikeUpRef(TEXT("/Script/Niagara.NiagaraSystem'/Game/PKH/NS/Vfx_Positive.Vfx_Positive'"));
	if(Vfx_LikeUpRef.Object)
	{
		Vfx_LikeUp = Vfx_LikeUpRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> Vfx_LikeDownRef(TEXT("/Script/Niagara.NiagaraSystem'/Game/PKH/NS/Vfx_Negative.Vfx_Negative'"));
	if (Vfx_LikeDownRef.Object)
	{
		Vfx_LikeDown = Vfx_LikeDownRef.Object;
	}
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

	SetNPCPatrol();

	// Init Greeting & Present Data
	if(CurLikeability >= FriendlyLikeability)
	{
		InitGreeting();
	}

	const FString DefaultPath = UKismetSystemLibrary::GetProjectDirectory() + TEXT("Extras/WavFiles/Default.wav");
	PlayTTS(DefaultPath);
}

#pragma region Start / End Conversation
void ANPCBase::StartConversation(bool IsStart)
{
	if(ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
	{
		NPCController->StartConversation();
		if (CanRotateInWorking() || false == NPCController->GetIsWorking())
		{
			AnimInstance->PlayMontage_Conv();
		}
		EmotionUI->SetVisibility(ESlateVisibility::Hidden);

		if(IsStart && false == Portraits_Default.IsEmpty())
		{
			const int32 Idx = FMath::RandRange(0, Portraits_Default.Num() - 1);
			MyGameMode->UpdatePortrait(Portraits_Default[Idx]);
		}
	}
}

void ANPCBase::EndConversation()
{
	EmotionUI->SetVisibility(ESlateVisibility::Hidden);
	NPCController->EndConversation();
	if (CanRotateInWorking() || AnimInstance->GetCurrentActiveMontage() != Montage_Work)
	{
		AnimInstance->StopAllMontages(0);
	}
	SfxComp->Stop();

	if(NPCController->IsMovingSomewhere())
	{
		SetNPCWalk();
	}
	else
	{
		SetNPCPatrol();
	}
	OnConversationEnd();
}

void ANPCBase::OnConversationEnd()
{
}
#pragma endregion


#pragma region TTS
void ANPCBase::SetCurEmotion(const FString& NewEmotion)
{
	CurEmotion = NewEmotion;
	SetCurPortrait();
}

void ANPCBase::SetCurEmotion(EEmotion NewEmotion)
{
	CurEmotion = UEnum::GetValueAsString(NewEmotion).Mid(10);
	SetCurPortrait();
}

void ANPCBase::PlayEmotion(bool IsUIOnly)
{
	if(CurEmotion.IsEmpty())
	{
		return;
	}

	if(nullptr != Vfx_CurLike)
	{
		VfxComp->SetAsset(Vfx_CurLike);
		VfxComp->ActivateSystem();
		Vfx_CurLike = nullptr;
	}

	EmotionUI->SetEmotion(CurEmotion);
	EmotionUI->SetVisibility(ESlateVisibility::Visible);
	if(IsUIOnly)
	{
		if(Sfx_Notice)
		{
			SfxComp->SetSound(Sfx_Notice);
			SfxComp->Play();
		}
	}
	else
	{
		if(CanRotateInWorking() || false ==  NPCController->GetIsWorking())
		{
			AnimInstance->PlayMontage_Emotion(CurEmotion);
		}
		if(nullptr != CurPortrait)
		{
			MyGameMode->UpdatePortrait(CurPortrait);
		}

		if (CurEmotion == "joy" && IsValid(Sfx_Joy))
		{
			SfxComp->SetSound(Sfx_Joy);
		}
		else if(CurEmotion == "anger" && IsValid(Sfx_Anger))
		{
			SfxComp->SetSound(Sfx_Anger);
		}
		else if (CurEmotion == "sadness" && IsValid(Sfx_Sad))
		{
			SfxComp->SetSound(Sfx_Sad);
		}
		else if (CurEmotion == "surprise" && IsValid(Sfx_Surprise))
		{
			SfxComp->SetSound(Sfx_Surprise);
		}
		else if (IsValid(Sfx_Indiff))
		{
			SfxComp->SetSound(Sfx_Indiff);
		}

		if(SfxComp->Sound)
		{
			SfxComp->Play();
		}
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
void ANPCBase::SetNPCPatrol()
{
	GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;
}

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
	StartConversation(true);

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

	if(InLikeability > 0 && nullptr != Vfx_LikeUp)
	{
		Vfx_CurLike = Vfx_LikeUp;
	}
	else if(InLikeability < 0 && nullptr != Vfx_LikeDown)
	{
		Vfx_CurLike = Vfx_LikeDown;
	}

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
	bool bIsPrefer = PreferItemName.Contains(ItemName);
	UpdateLikeability(bIsPrefer ? PreferItemValue : NormalItemValue);

	// 통신
	AFarmLifeGameMode* GameMode = CastChecked<AFarmLifeGameMode>(GetWorld()->GetAuthGameMode());
	GameMode->RequestPresentData(this, bIsPrefer);
}
#pragma endregion

# pragma region for override
void ANPCBase::DoJob()
{
	SetActorRotation(WorkRotation);
	if (AnimInstance->GetCurrentActiveMontage() != Montage_Work)
	{
		AnimInstance->PlayMontage_Custom(Montage_Work);
	}
}

void ANPCBase::StartSit()
{
}

void ANPCBase::EndSit()
{
}

void ANPCBase::StandUp()
{
}

bool ANPCBase::CanRotateInWorking()
{
	return true;
}
#pragma endregion

void ANPCBase::SetCurPortrait()
{
	int32 Idx = 0;

	if(CurEmotion == UEnum::GetValueAsString(EEmotion::joy).Mid(10))
	{
		if (Portraits_Joy.IsEmpty())
		{
			return;
		}
		Idx = FMath::RandRange(0, Portraits_Joy.Num() - 1);
		CurPortrait = Portraits_Joy[Idx];
	}
	else if (CurEmotion == UEnum::GetValueAsString(EEmotion::surprise).Mid(10))
	{
		if (Portraits_Surprise.IsEmpty())
		{
			return;
		}
		Idx = FMath::RandRange(0, Portraits_Surprise.Num() - 1);
		CurPortrait = Portraits_Surprise[Idx];
	}
	else if (CurEmotion == UEnum::GetValueAsString(EEmotion::sadness).Mid(10))
	{
		if (Portraits_Sadness.IsEmpty())
		{
			return;
		}
		Idx = FMath::RandRange(0, Portraits_Sadness.Num() - 1);
		CurPortrait = Portraits_Sadness[Idx];
	}
	else if (CurEmotion == UEnum::GetValueAsString(EEmotion::anger).Mid(10))
	{
		if (Portraits_Anger.IsEmpty())
		{
			return;
		}
		Idx = FMath::RandRange(0, Portraits_Anger.Num() - 1);
		CurPortrait = Portraits_Anger[Idx];
	}
	else
	{
		if (Portraits_Default.IsEmpty())
		{
			return;
		}
		Idx = FMath::RandRange(0, Portraits_Default.Num() - 1);
		CurPortrait = Portraits_Default[Idx];
	}
}

void ANPCBase::StopAI()
{
	if(NPCController)
	{
		NPCController->StopAI();
	}
}

void ANPCBase::OnDateUpdated(int32 NewDate)
{
	if (CurLikeability >= FriendlyLikeability)
	{
		InitGreeting();
	}

	SetActorLocation(HomeLoc);
	NPCController->ResetBBKeys();
}
