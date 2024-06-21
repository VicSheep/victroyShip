// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/Test/TextInputComponent.h"

#include "Blueprint/UserWidget.h"
#include "Components/EditableText.h"
#include "JIU/FarmerComponent.h"
#include "JIU/PlantInfoWidget.h"
#include "PKH/Component/TalkComponent.h"
#include "PKH/Game/FarmLifeGameMode.h"
#include "PKH/UI/ChatUIWidget.h"

UTextInputComponent::UTextInputComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	static ConstructorHelpers::FClassFinder<UChatUIWidget> ChatUIClassRef(TEXT("/Game/PKH/UI/WBP_ChatUI.WBP_ChatUI_C"));
	if(ChatUIClassRef.Class)
	{
		ChatUIClass = ChatUIClassRef.Class;
	}
}

void UTextInputComponent::BeginPlay()
{
	Super::BeginPlay();

	MyGameMode = Cast< AFarmLifeGameMode>(GetWorld()->GetAuthGameMode());
	ensure(MyGameMode);

	ChatUI = CreateWidget<UChatUIWidget>(GetWorld(), ChatUIClass);
	ensure(ChatUI);

	ChatUI->AddToViewport(2);
	ChatUI->SetVisibility(ESlateVisibility::Hidden);
	ChatUI->GetChatWidget()->OnTextCommitted.AddDynamic(this, &UTextInputComponent::OnChatTextCommitted);

	// FarmerComp
	FarmerComp = Cast<UFarmerComponent>(GetOwner()->GetComponentByClass(UFarmerComponent::StaticClass()));
	ensure(FarmerComp);
}

#pragma region Text Input
void UTextInputComponent::Chat()
{
	if(false == InChatting)
	{
		ChatUI->SetVisibility(ESlateVisibility::Visible);
		ChatUI->Focus();
		InChatting = true;
		MyGameMode->ChangeInputMode_Both();
	}
	else
	{
		ChatUI->SetVisibility(ESlateVisibility::Hidden);
		InChatting = false;
		if(FarmerComp->PlantInfoWidget->IsVisible() || MyGameMode->IsInConversation())
		{
			MyGameMode->ChangeInputMode_Game();
			MyGameMode->ChangeInputMode_Both();
		}
		else
		{
			MyGameMode->ChangeInputMode_Game();
		}

		FString InputText = ChatUI->GetChatText();
		if (InputText.IsEmpty())
		{
			return;
		}

		UTalkComponent* TalkComp = Cast<UTalkComponent>(GetOwner()->GetComponentByClass(UTalkComponent::StaticClass()));
		if (TalkComp)
		{
			TalkComp->CheckNearbyObjects(InputText);
		}
	}
}

bool UTextInputComponent::IsChatting() const
{
	return InChatting;
}

void UTextInputComponent::OnChatTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if(false == InChatting)
	{
		return;
	}

	if (CommitMethod == ETextCommit::OnEnter)
	{
		Chat();
	}
}
#pragma endregion
