// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/Game/FarmLifeGameMode.h"

#include "Engine/DirectionalLight.h"
#include "JIU/PlantActor.h"
#include "Kismet/GameplayStatics.h"
#include "PKH/Http/HttpActor.h"
#include "PKH/Interface/DateUpdate.h"
#include "PKH/Interface/HourUpdate.h"
#include "PKH/NPC/NPCBase.h"
#include "PKH/UI/NPCConversationWidget.h"
#include "PKH/UI/TimerWidget.h"
#include "Serialization/EditorBulkData.h"
#include "YSH/skySystem.h"

#define TEN_MINUTES 10
#define SIXTY_MINUTES 60
#define START_HOUR 8
#define END_HOUR 18
#define TIME_UPDATE_INTERVAL 5.0f

AFarmLifeGameMode::AFarmLifeGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	// Default Pawn & Controller


	// Http Actor
	static ConstructorHelpers::FClassFinder<AHttpActor> HttpActorClassRef(TEXT("/Game/PKH/Blueprint/BP_HttpActor.BP_HttpActor_C"));
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
}

void AFarmLifeGameMode::BeginPlay()
{
	Super::BeginPlay();

	if(HttpActorClass)
	{
		HttpActor = GetWorld()->SpawnActor<AHttpActor>(HttpActorClass);
	}

	// Player Home Loc
	if(const AActor* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
	{
		PlayerHomLoc = Player->GetActorLocation();
	}

	// Time flow
	SunLight = UGameplayStatics::GetActorOfClass(GetWorld(), ADirectionalLight::StaticClass());
	if(nullptr == SunLight)
	{
		SunLight = UGameplayStatics::GetActorOfClass(GetWorld(), AskySystem::StaticClass());
	}
	if(SunLight)
	{
		SunLight->SetActorRotation(SunBeginRot);
	}

	GetWorldTimerManager().SetTimer(TimerHandle, this, &AFarmLifeGameMode::UpdateMinutes, TIME_UPDATE_INTERVAL, true, TIME_UPDATE_INTERVAL);

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
	ConversationUI->AddToViewport();
	ConversationUI->SetVisibility(ESlateVisibility::Hidden);
}

void AFarmLifeGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (SunLight && false == Paused)
	{
		SunLight->AddActorWorldRotation(SunDeltaRot * DeltaSeconds);
	}
}

#pragma region NPC conversation
void AFarmLifeGameMode::SendSpeech(const FString& FileName, const FString& FilePath, const TObjectPtr<ANPCBase>& NewNPC)
{
	CurNPC = NewNPC;
	CurNPC->StartConversation();

	HttpActor->SendSpeech(FileName, FilePath);
	ConversationUI->SetVisibility(ESlateVisibility::Visible);
	ConversationUI->UpdateConversationUI(CurNPC->GetNPCName(), TEXT("플레이어의 입력을 처리중입니다..."));
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
		ConversationUI->UpdateConversationUI(CurNPC->GetNPCName(), LatestSpeech, true);
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
}

void AFarmLifeGameMode::ShowPlayerText(const FString& PlayerInputText)
{
	if(CurNPC)
	{
		ConversationUI->UpdateConversationUI(TEXT("플레이어"), PlayerInputText, true);
	}
}

// By Text
void AFarmLifeGameMode::SendText(const FString& InputText, const TObjectPtr<ANPCBase>& NewNPC)
{
	CurNPC = NewNPC;
	CurNPC->StartConversation();
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
	if(CurNPC)
	{
		CurNPC->PlayTTS(FilePath);
	}
}
#pragma endregion

#pragma region Talk to plant
void AFarmLifeGameMode::TalkToPlant(const FString& FileName, const FString& FilePath, const TArray<TObjectPtr<APlantActor>>& NewPlants)
{
	CurPlants = NewPlants;
	HttpActor->TalkToPlant(FileName, FilePath);
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
void AFarmLifeGameMode::InitGreeting(const FString& NPCName, const FString& NPCText, int32 Likeability)
{
	HttpActor->InitGreeting(NPCName, NPCText, Likeability);
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

	ConversationUI->UpdateConversationUI(CurNPC->GetNPCName(), NPCResponse.Answer, true, true);
	CurNPC->PlayTTS(NPCResponse.FilePath);
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
