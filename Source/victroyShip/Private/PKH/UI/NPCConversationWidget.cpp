// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/UI/NPCConversationWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "PKH/Component/TalkComponent.h"
#include "PKH/Game/FarmLifeGameMode.h"
#include "PKH/NPC/NPCBase.h"

UNPCConversationWidget::UNPCConversationWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

void UNPCConversationWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MyGameMode = CastChecked<AFarmLifeGameMode>(GetWorld()->GetAuthGameMode());

	Btn_Exit->OnClicked.AddDynamic(this, &UNPCConversationWidget::OnClicked_Exit);
	OnVisibilityChanged.AddDynamic(this, &UNPCConversationWidget::OnHidden);
}

void UNPCConversationWidget::UpdatePortrait(class UTexture2D* NewPortrait)
{
	Img_Portrait->SetBrushFromTexture(NewPortrait);
}

void UNPCConversationWidget::OnClicked_Exit()
{
	MyGameMode->EndConversation();
	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if(Player)
	{
		UTalkComponent* TalkComp = Cast<UTalkComponent>(Player->GetComponentByClass(UTalkComponent::StaticClass()));
		if(TalkComp)
		{
			TalkComp->EndConversation();
		}
	}

	SetVisibility(ESlateVisibility::Hidden);
	MyGameMode->CheckDateUpdate();
	MyGameMode->ChangeInputMode_Game();
}

void UNPCConversationWidget::OnHidden(ESlateVisibility InVisibility)
{
	if(InVisibility != ESlateVisibility::Hidden)
	{
		return;
	}

	CurConvState = EConvState::None;
	Txt_Conversation->SetText(FText::FromString(TEXT("")));

	// 텍스트 스트림 중이라면 해제
	if (GetWorld()->GetTimerManager().IsTimerActive(StreamHandle)) 
	{
		GetWorld()->GetTimerManager().ClearTimer(StreamHandle);
		UE_LOG(LogTemp, Error, TEXT("Stream Stop"));
	}
}

void UNPCConversationWidget::UpdateConversationUI(const FString& NewConversation, bool DoStream, bool FromNPC)
{
	if(DoStream)
	{
		if(CurConvState != EConvState::None)
		{
			NextText = NewConversation;
		}
		else
		{
			CurText = NewConversation; 
			CurLen = 1;
			CurConvState = FromNPC ? EConvState::NPC : EConvState::Player;
			Txt_NPCName->SetText(FromNPC ? FText::FromString(MyGameMode->GetCurNPC()->GetNPCName()) : FText::FromString(TEXT("플레이어")));
			GetWorld()->GetTimerManager().SetTimer(StreamHandle, this, &UNPCConversationWidget::StreamText, StreamDeltaTime, true);
		}
	}
	else
	{
		Txt_Conversation->SetText(FText::FromString(NewConversation));
		Txt_NPCName->SetText(FText::FromString(TEXT("플레이어")));
		CurConvState = EConvState::None;
	}
}

void UNPCConversationWidget::StreamText()
{
	if(CurConvState != EConvState::Wait && CurLen <= CurText.Len())
	{
		Txt_Conversation->SetText(FText::FromString(CurText.Mid(0, CurLen)));
		++CurLen;
		return;
	}
	
	CurWaitTime += StreamDeltaTime;
	if (CurWaitTime < MaxWaitTime)
	{
		return;
	}

	CurWaitTime = 0;

	switch(CurConvState)
	{
	case EConvState::Player:
		CurConvState = EConvState::Wait;
		if (MyGameMode->GetCurNPC())
		{
			Txt_NPCName->SetText(FText::FromString(MyGameMode->GetCurNPC()->GetNPCName()));
		}
		Txt_Conversation->SetText(FText::FromString(FString::Printf(TEXT("%s(이)가 답변을 고민중입니다."), *MyGameMode->GetCurNPC()->GetNPCName() )));
		break;
	case EConvState::Wait:
		CurConvState = EConvState::NPC;
		MyGameMode->PlayNPCEmotion();
		CurText = NextText;
		NextText = TEXT("");
		CurLen = 1;
		break;
	case EConvState::NPC:
		CurConvState = EConvState::None;
		GetWorld()->GetTimerManager().ClearTimer(StreamHandle);
		break;
	default:
		break;
	}
}

void UNPCConversationWidget::PlayNow()
{
	// if waiting tts, play text now
	if(CurConvState == EConvState::Wait && false == NextText.IsEmpty())
	{
		CurConvState = EConvState::NPC;
		MyGameMode->PlayNPCEmotion();
		CurText = NextText;
		NextText = TEXT("");

		CurWaitTime = 0;
		CurLen = 1;
	}
}

bool UNPCConversationWidget::CanMoveToNextTalk()
{
	return CurConvState == EConvState::None;
}

void UNPCConversationWidget::NoticeForWaiting()
{
	if(false == GetWorld()->GetTimerManager().IsTimerActive(NoticeHandle))
	{
		Txt_Notice->SetVisibility(ESlateVisibility::Visible);
		GetWorld()->GetTimerManager().SetTimer(NoticeHandle, this, &UNPCConversationWidget::SetNoticeHidden, 3.0f, false);
	}
}

void UNPCConversationWidget::SetNoticeHidden()
{
	Txt_Notice->SetVisibility(ESlateVisibility::Hidden);
}