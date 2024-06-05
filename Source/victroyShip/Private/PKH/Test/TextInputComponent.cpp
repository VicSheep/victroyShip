// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/Test/TextInputComponent.h"

#include "Blueprint/UserWidget.h"
#include "Components/EditableText.h"
#include "Kismet/GameplayStatics.h"
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

	ChatUI = CreateWidget<UChatUIWidget>(GetWorld(), ChatUIClass);
	ensure(ChatUI);
	ChatUI->AddToViewport();
	ChatUI->SetVisibility(ESlateVisibility::Hidden);
	ChatUI->GetChatWidget()->OnTextCommitted.AddDynamic(this, &UTextInputComponent::OnChatTextCommitted);
}

#pragma region 채팅 입력
void UTextInputComponent::Chat()
{
	if(false == InChatting)
	{
		ChatUI->SetVisibility(ESlateVisibility::Visible);
		ChatUI->Focus();
		InChatting = true;
	}
	else
	{
		ChatUI->SetVisibility(ESlateVisibility::Hidden);
		InChatting = false;

		if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			FInputModeGameAndUI InputMode;
			PC->SetInputMode(InputMode);
		}

		FString InputText = ChatUI->GetChatText();
		if (InputText.IsEmpty())
		{
			return;
		}

		UTalkComponent* STTComp = Cast<UTalkComponent>(GetOwner()->GetComponentByClass(UTalkComponent::StaticClass()));
		if (STTComp)
		{
			STTComp->CheckNearbyObjects(InputText);
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

	UE_LOG(LogTemp, Warning, TEXT("GetEnter"))
	if (CommitMethod == ETextCommit::OnEnter)
	{
		Chat();
	}
}
#pragma endregion
