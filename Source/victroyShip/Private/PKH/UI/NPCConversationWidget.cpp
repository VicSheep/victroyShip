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

void UNPCConversationWidget::UpdateConversationUI(const FString& NPCName, const FString& NewConversation, bool DoStream)
{
	Txt_NPCName->SetText(FText::FromString(NPCName));

	if(DoStream)
	{
		if(IsStreaming) // 아직 스트리밍 중이라면 다음 텍스트로 저장
		{
			NextText = NewConversation;
		}
		else
		{
			CurText = NewConversation;
			CurLen = 1;
			IsStreaming = true;
			GetWorld()->GetTimerManager().SetTimer(StreamHandle, this, &UNPCConversationWidget::StreamText, StreamDeltaTime, true);
		}
	}
	else
	{
		Txt_Conversation->SetText(FText::FromString(NewConversation));
	}
}

void UNPCConversationWidget::StreamText()
{
	if(CurLen <= CurText.Len())
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
	
	if(NextText.IsEmpty())
	{
		CurWaitTime = 0;
		IsStreaming = false;
		GetWorld()->GetTimerManager().ClearTimer(StreamHandle);
	}
	else
	{
		CurText = NextText;
		NextText = TEXT("");
		Txt_Conversation->SetText(FText::FromString(FString::Printf(TEXT("%s(이)가 답변을 고민중입니다."), *MyGameMode->GetCurNPC()->GetNPCName() )));

		CurWaitTime = 0;
		CurLen = 1;
	}
}
