// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NPCConversationWidget.generated.h"

UENUM()
enum class EConvState : uint8
{
	None = 0,
	Player,
	Wait,
	NPC
};

/**
 * 
 */
UCLASS()
class VICTROYSHIP_API UNPCConversationWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UNPCConversationWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class AFarmLifeGameMode> MyGameMode;

protected:
	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	TObjectPtr<class UTextBlock> Txt_NPCName;

	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	TObjectPtr<class UTextBlock> Txt_Conversation;

	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	TObjectPtr<class UImage> Img_Portrait;

	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	TObjectPtr<class UButton> Btn_Exit;

public:
	UFUNCTION()
	void OnClicked_Exit();

	void UpdateConversationUI(const FString& NPCName, const FString& NewConversation, bool DoStream = false);

// Stream
protected:
	EConvState CurConvState = EConvState::None;

	const float StreamDeltaTime = 0.1f;
	FTimerHandle StreamHandle;

	int32 CurLen = 1;
	FString CurText = TEXT("");
	FString NextText = TEXT("");

	const float MaxWaitTime = 2.0f;
	float CurWaitTime = 0.0f;

	UFUNCTION()
	void StreamText();

};
