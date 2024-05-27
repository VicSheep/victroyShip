// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/UI/NPCConversationWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
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
}

void UNPCConversationWidget::OnClicked_Exit()
{
	MyGameMode->EndConversation();
	SetVisibility(ESlateVisibility::Hidden);
	MyGameMode->CheckDateUpdate();
}

void UNPCConversationWidget::UpdateConversationUI(const FString& NPCName, const FString& NewConversation, bool DoStream, bool FromNPC)
{
	Txt_NPCName->SetText(FText::FromString(NPCName));

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
			GetWorld()->GetTimerManager().SetTimer(StreamHandle, this, &UNPCConversationWidget::StreamText, StreamDeltaTime, true);
		}
	}
	else
	{
		Txt_Conversation->SetText(FText::FromString(NewConversation));
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
