// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/UI/NPCConversationWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

UNPCConversationWidget::UNPCConversationWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UNPCConversationWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Btn_Exit->OnClicked.AddDynamic(this, &UNPCConversationWidget::OnClicked_Exit);
}

void UNPCConversationWidget::OnClicked_Exit()
{
	SetVisibility(ESlateVisibility::Hidden);
}

void UNPCConversationWidget::UpdateConversationUI(const FString& NPCName, const FString& NewConversation)
{
	Txt_NPCName->SetText(FText::FromString(NPCName));

	const FString& ConversationText = NewConversation.IsEmpty() ? TEXT("AI 답변을 기다리는 중입니다...") : NewConversation;
	Txt_Conversation->SetText(FText::FromString(ConversationText));
}
