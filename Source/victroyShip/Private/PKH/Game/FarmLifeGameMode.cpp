// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/Game/FarmLifeGameMode.h"

#include "Components/AudioComponent.h"
#include "Engine/DirectionalLight.h"
#include "JIU/PlantActor.h"
#include "Kismet/GameplayStatics.h"
#include "PKH/Http/NewHttpActor.h"
#include "PKH/Interface/DateUpdate.h"
#include "PKH/Interface/HourUpdate.h"
#include "PKH/NPC/NPCBase.h"
#include "PKH/UI/EndingUI_Fail.h"
#include "PKH/UI/EndingUI_Success.h"
#include "PKH/UI/NPCConversationWidget.h"
#include "PKH/UI/TimerWidget.h"
#include "Sound/AmbientSound.h"
#include "YSH/skySystem.h"

#define TEN_MINUTES 10
#define SIXTY_MINUTES 60
#define START_HOUR 8
#define END_HOUR 18

AFarmLifeGameMode::AFarmLifeGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	// Default Pawn & Controller


	// Http Actor
	static ConstructorHelpers::FClassFinder<ANewHttpActor> HttpActorClassRef(TEXT("/Game/PKH/Blueprint/BP_NewHttpActor.BP_NewHttpActor_C"));
	if(HttpActorClassRef.Class)
	{
		HttpActorClass = HttpActorClassRef.Class;
	}

	// UI
	static ConstructorHelpers::FClassFinder<UNPCConversationWidget> ConversationUIClassRef(TEXT("/Game/PKH/UI/WBP_NPCConversation.WBP_NPCConversation_C"));
	if (ConversationUIClassRef.Class)
	{
		ConversationUIClass = ConversationUIClassRef.Class;
	}
	static ConstructorHelpers::FClassFinder<UTimerWidget> TimerUIClassRef(TEXT("/Game/PKH/UI/WBP_Timer.WBP_Timer_C"));
	if (TimerUIClassRef.Class)
	{
		TimerUIClass = TimerUIClassRef.Class;
	}

	// Sound
	static ConstructorHelpers::FObjectFinder<USoundBase> BGM_BackToPortlandRef(TEXT("/Script/Engine.SoundWave'/Game/PKH/Sound/BGM_BackToPortland.BGM_BackToPortland'"));
	if (BGM_BackToPortlandRef.Object)
	{
		BGM_BackToPortland = BGM_BackToPortlandRef.Object;
	}

	// For Ending
	static ConstructorHelpers::FClassFinder<UEndingUI_Fail> EndingUI_FailClassRef(TEXT("/Game/PKH/UI/WBP_EndingUI_Fail.WBP_EndingUI_Fail_C"));
	if (EndingUI_FailClassRef.Class)
	{
		EndingUI_FailClass = EndingUI_FailClassRef.Class;
	}
	static ConstructorHelpers::FClassFinder<UEndingUI_Success> EndingUI_SuccessClassRef(TEXT("/Game/PKH/UI/WBP_EndingUI_Success.WBP_EndingUI_Success_C"));
	if (EndingUI_SuccessClassRef.Class)
	{
		EndingUI_SuccessClass = EndingUI_SuccessClassRef.Class;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> BGM_EndingFailRef(TEXT("/Script/Engine.SoundWave'/Game/PKH/Sound/BGM_EndingFail.BGM_EndingFail'"));
	if (BGM_EndingFailRef.Object)
	{
		BGM_EndingFail = BGM_EndingFailRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> BGM_EndingSuccessRef(TEXT("/Script/Engine.SoundWave'/Game/PKH/Sound/BGM_EndingSuccessShort.BGM_EndingSuccessShort'"));
	if (BGM_EndingSuccessRef.Object)
	{
		BGM_EndingSuccess = BGM_EndingSuccessRef.Object;
	}
}

void AFarmLifeGameMode::BeginPlay()
{
	Super::BeginPlay();

	if(HttpActorClass)
	{
		HttpActor = GetWorld()->SpawnActor<ANewHttpActor>(HttpActorClass);
	}

	MyController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	ensure(MyController);
	FTimerHandle TempHandle;
	GetWorldTimerManager().SetTimer(TempHandle, this, &AFarmLifeGameMode::ChangeInputMode_Game, 0.1f, false);

	// Player Home Loc
	if(const AActor* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
	{
		PlayerHomLoc = Player->GetActorLocation();
	}

	// Time flow
	/*SunLight = UGameplayStatics::GetActorOfClass(GetWorld(), ADirectionalLight::StaticClass());
	if(nullptr == SunLight)
	{
		SunLight = UGameplayStatics::GetActorOfClass(GetWorld(), AskySystem::StaticClass());
	}
	if(SunLight)
	{
		SunLight->SetActorRotation(SunBeginRot);
	}*/

	GetWorldTimerManager().SetTimer(TimerHandle, this, &AFarmLifeGameMode::UpdateMinutes, TimeUpdateInterval, true, TimeUpdateInterval);

	// UI
	TimerUI = CreateWidget<UTimerWidget>(GetWorld(), TimerUIClass);
	ensure(TimerUI);
	TimerUI->AddToViewport();
	TimerUI->UpdateTimerUI(Date, Hours, Minutes);
	TimerUI->FadeOutFinished.BindDynamic(this, &AFarmLifeGameMode::OnNextDay);
	TimerUI->FadeInFinished.BindDynamic(this, &AFarmLifeGameMode::OnFadeOutFinished);
	TimerUI->BindOnFinished();

	ConversationUI = CreateWidget<UNPCConversationWidget>(GetWorld(), ConversationUIClass);
	ensure(ConversationUI);
	ConversationUI->AddToViewport(1);
	ConversationUI->SetVisibility(ESlateVisibility::Hidden);

	// Sound
	BGMComp = UGameplayStatics::SpawnSound2D(GetWorld(), BGM_BackToPortland, 0.6f, 1, 0, nullptr, false, false);
	BGMComp->FadeIn(3.0f, 0.6f);
}

void AFarmLifeGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	/*if (SunLight && false == Paused)
	{
		SunLight->AddActorWorldRotation(SunDeltaRot * DeltaSeconds);
	}*/
}

#pragma region NPC conversation
void AFarmLifeGameMode::SendSpeech(const FString& FileName, const FString& FilePath, const TObjectPtr<ANPCBase>& NewNPC)
{
	if(false == CanTalkOrPresent())
	{
		return;
	}

	bool IsStart = (CurNPC == nullptr);
	CurNPC = NewNPC;
	CurNPC->StartConversation(IsStart);
	CurNPC->SetEmotionUI(false);
	BGMComp->SetVolumeMultiplier(BgmMultiplier_Conv);

	HttpActor->SendSpeech(FilePath);
	ConversationUI->SetVisibility(ESlateVisibility::Visible);
	ConversationUI->UpdateConversationUI(TEXT("플레이어의 입력을 처리중입니다..."));
}

void AFarmLifeGameMode::SetLatestSpeech(const FNPCResponse& Response)
{
	if(nullptr == CurNPC)
	{
		return;
	}

	LatestSpeech = Response.Answer;

	// UI 갱신
	if(ConversationUI->IsVisible())
	{
		ConversationUI->UpdateConversationUI(LatestSpeech, true);
		CurNPC->SetCurEmotion(Response.Emotion);
	}

	// 호감도 갱신
	CurNPC->UpdateLikeability(Response.Likeability);
	UE_LOG(LogTemp, Warning, TEXT("[%s's likeability] : %d"), *CurNPC->GetNPCName(), CurNPC->GetLikeability());
	UE_LOG(LogTemp, Warning, TEXT("[%s's emotion] : %s"), *CurNPC->GetNPCName(), *Response.Emotion);
}

FString& AFarmLifeGameMode::GetLatestSpeech()
{
	return LatestSpeech;
}

void AFarmLifeGameMode::EndConversation()
{
	if(nullptr == CurNPC)
	{
		return;
	}

	CurNPC->EndConversation();
	HttpActor->EndChat(CurNPC->GetNPCName());
	CurNPC = nullptr;
	BGMComp->SetVolumeMultiplier(BgmMultiplier_Normal);
}

void AFarmLifeGameMode::ShowPlayerText(const FString& PlayerInputText)
{
	if(CurNPC)
	{
		ConversationUI->UpdateConversationUI(PlayerInputText, true);
	}
}

// By Text
void AFarmLifeGameMode::SendText(const FString& InputText, const TObjectPtr<ANPCBase>& NewNPC)
{
	if (false == CanTalkOrPresent())
	{
		return;
	}

	const bool IsStart = (CurNPC == nullptr);
	CurNPC = NewNPC;
	CurNPC->StartConversation(IsStart);
	CurNPC->SetEmotionUI(false);
	ShowPlayerText(InputText);

	HttpActor->SendText(CurNPC->GetNPCName(), InputText, CurNPC->GetLikeability());
	ConversationUI->SetVisibility(ESlateVisibility::Visible);
}

void AFarmLifeGameMode::PlayNPCEmotion()
{
	if(CurNPC)
	{
		CurNPC->PlayEmotion();
	}
}

void AFarmLifeGameMode::PlayTTS(const FString& FilePath)
{
	// NPC가 말하는 상황에서만 실행
	if(ConversationUI->IsVisible() && nullptr != CurNPC)
	{
		ConversationUI->PlayNow();
		if(ConversationUI->CanPlayTTS())
		{
			CurNPC->PlayTTS(FilePath);
		}
		else
		{
			CurTTSFilePath = FilePath;
		}
	}
}

void AFarmLifeGameMode::PlayReservedTTS()
{
	if(CurTTSFilePath.IsEmpty())
	{
		return;
	}

	CurNPC->PlayTTS(CurTTSFilePath);
	CurTTSFilePath = TEXT("");
}

bool AFarmLifeGameMode::IsInConversation()
{
	return ConversationUI->IsVisible();
}
#pragma endregion

#pragma region Talk to plant
void AFarmLifeGameMode::TalkToPlant(const FString& FileName, const FString& FilePath, const TArray<TObjectPtr<APlantActor>>& NewPlants)
{
	CurPlants = NewPlants;
	//HttpActor->TalkToPlant(FileName, FilePath);
	HttpActor->TalkToPlant(FilePath);
}

void AFarmLifeGameMode::SetTalkScore(int32 Score)
{
	if(Score == 0)
	{
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("Cur Plants Size: %d"), CurPlants.Num());
	const bool IsPositive = Score > 0;
	for(APlantActor* P : CurPlants)
	{
		// 긍정적이라면 작물 성장
		if(IsPositive)
		{
			P->GrowPlant(); UE_LOG(LogTemp, Warning, TEXT("Grow Plant"));
		}
	}
}

int32 AFarmLifeGameMode::GetTalkScore()
{
	return TalkScore;
}

void AFarmLifeGameMode::TalkToPlantWithText(const FString& InputText, const TArray<TObjectPtr<APlantActor>>& NewPlants)
{
	CurPlants = NewPlants;
	HttpActor->TalkToPlantWithText(InputText);
}
#pragma endregion

#pragma region Greeting
void AFarmLifeGameMode::InitGreeting(const FString& NPCName, int32 Likeability)
{
	HttpActor->InitGreeting(NPCName, Likeability);
}

void AFarmLifeGameMode::RequestGreetingData(class ANPCBase* NewNPC)
{
	CurNPC = NewNPC;
	HttpActor->RequestGreeting(CurNPC->GetNPCName());
	ConversationUI->SetVisibility(ESlateVisibility::Visible);
}

void AFarmLifeGameMode::GreetingToPlayer(const FNPCResponse& NPCResponse)
{
	if(nullptr == CurNPC)
	{
		return;
	}

	ConversationUI->UpdateConversationUI(NPCResponse.Answer, true, true);
	ChangeInputMode_Both();
}
#pragma endregion

#pragma region Present
void AFarmLifeGameMode::RequestPresentData(ANPCBase* NewNPC, bool IsPrefer)
{
	CurNPC = NewNPC;
	HttpActor->RequestPresent(CurNPC->GetNPCName(), CurNPC->GetLikeability(), IsPrefer);

	ConversationUI->UpdateConversationUI(TEXT(""), false, true);
	ConversationUI->SetVisibility(ESlateVisibility::Visible);
	ChangeInputMode_Both();
}

void AFarmLifeGameMode::ResponseToPlayerForPresent(const FNPCResponse& NPCResponse)
{
	if (nullptr == CurNPC)
	{
		return;
	}

	ConversationUI->UpdateConversationUI(NPCResponse.Answer, true, true);
	CurNPC->ResponseToPresent();
}
#pragma endregion

#pragma region Time flow
void AFarmLifeGameMode::StartTime()
{
	GetWorldTimerManager().UnPauseTimer(TimerHandle);
	Paused = false;
}

void AFarmLifeGameMode::StopTime()
{
	GetWorldTimerManager().PauseTimer(TimerHandle);
	Paused = true;
}

// For Binding
void AFarmLifeGameMode::OnNextDay()
{
	Hours = START_HOUR;
	Minutes = 0;
	++Date;
	if(Date == MaxDate)
	{
		EndGame();
		return;
	}

	if (SunLight)
	{
		SunLight->SetActorRotation(SunBeginRot);
	}

	// 플레이어 위치 초기화
	if(AActor* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
	{
		Player->SetActorLocation(PlayerHomLoc);
	}

	// 날짜 업데이트 일괄처리
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UDateUpdate::StaticClass(), OutActors);
	for (AActor* Actor : OutActors)
	{
		IDateUpdate* DU = Cast<IDateUpdate>(Actor);
		DU->OnDateUpdated(Date);
	}

	TimerUI->UpdateTimerUI(Date, Hours, Minutes);
	TimerUI->StartFadeInAnim();
}
// For Binding
void AFarmLifeGameMode::OnFadeOutFinished()
{
	StartTime();
}

void AFarmLifeGameMode::CheckDateUpdate()
{
	if(Paused && Hours == END_HOUR)
	{
		UpdateDate();
	}
}

void AFarmLifeGameMode::UpdateMinutes()
{
	Minutes += TEN_MINUTES;
	
	if(Minutes == SIXTY_MINUTES)
	{
		UpdateHours();
	}
	TimerUI->UpdateTimerUI(Date, Hours, Minutes);
}

void AFarmLifeGameMode::UpdateHours()
{
	Minutes = 0;
	++Hours;

	// 시간 업데이트 일괄처리
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UHourUpdate::StaticClass(), OutActors);
	for(AActor* Actor : OutActors)
	{
		IHourUpdate* HU = Cast<IHourUpdate>(Actor);
		HU->OnHourUpdated(Hours);
	}

	if(Hours == END_HOUR)
	{
		UpdateDate();
	}
}

void AFarmLifeGameMode::UpdateDate()
{
	StopTime();

	// 대화중이라면 업데이트 보류
	if(ConversationUI->IsVisible())
	{
		return;
	}

	// 페이드아웃
	TimerUI->StartFadeOutAnim();
}
#pragma endregion

#pragma region InputMode
void AFarmLifeGameMode::ChangeInputMode_Game()
{
	MyController->SetInputMode(InputMode_Game);
	MyController->SetShowMouseCursor(false);
}

void AFarmLifeGameMode::ChangeInputMode_Both()
{
	MyController->SetInputMode(InputMode_Both);
	MyController->SetShowMouseCursor(true);
}
#pragma endregion

#pragma region UI & Post Process
void AFarmLifeGameMode::UpdatePortrait(UTexture2D* NewPortrait)
{
	ConversationUI->UpdatePortrait(NewPortrait);
}

void AFarmLifeGameMode::RecordOn()
{
	TimerUI->RecordOn();
}

void AFarmLifeGameMode::RecordOff()
{
	TimerUI->RecordOff();
}

bool AFarmLifeGameMode::CanTalkOrPresent()
{
	if(ConversationUI->CanMoveToNextTalk())
	{
		return true;
	}

	ConversationUI->NoticeForWaiting();
	return false;
}
#pragma endregion

#pragma region Ending
void AFarmLifeGameMode::EndGame()
{
	GetWorldTimerManager().ClearTimer(TimerHandle);
	TimerUI->FadeOutFinished.Clear();
	TimerUI->FadeInFinished.Clear();

	// Ambient Sound Off
	TArray<AActor*> AmbientArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAmbientSound::StaticClass(), AmbientArray);
	for(AActor* AmbActor : AmbientArray)
	{
		AAmbientSound* Amb = Cast<AAmbientSound>(AmbActor);
		Amb->GetAudioComponent()->Deactivate();
	}

	// NPC Count
	TArray<AActor*> NPCArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANPCBase::StaticClass(), NPCArray);

	int32 Count = 0;
	for(int i = 0; i < NPCArray.Num(); ++i)
	{
		ANPCBase* NPC = Cast<ANPCBase>(NPCArray[i]);
		if(NPC->GetLikeability() >= EndingSuccessLikeability)
		{
			++Count;
		}
		NPC->StopAI();
	}
	BGMComp->FadeOut(2.0f, 0);

	if(Count >= EndingSuccessCount)
	{
		EndingUI_Success = CreateWidget<UEndingUI_Success>(GetWorld(), EndingUI_SuccessClass);
		EndingUI_Success->AddToViewport(10);
		if(BGM_EndingSuccess)
		{
			BGMComp->SetSound(BGM_EndingSuccess);
			BGMComp->FadeIn(3.0f, 0.5f);
		}
	}
	else
	{
		EndingUI_Fail = CreateWidget<UEndingUI_Fail>(GetWorld(), EndingUI_FailClass);
		EndingUI_Fail->AddToViewport(10);
		if(BGM_EndingFail)
		{
			BGMComp->SetSound(BGM_EndingFail);
			BGMComp->FadeIn(3.0f, 0.5f);
		}
	}
}
#pragma endregion