// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/UI/NPCConversationWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "PKH/Game/FarmLifeGameMode.h"

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

void UNPCConversationWidget::UpdateConversationUI(const FString& NPCName, const FString& NewConversation)
{
	Txt_NPCName->SetText(FText::FromString(NPCName));
	Txt_Conversation->SetText(FText::FromString(NewConversation));
}
