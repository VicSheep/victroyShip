// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NPCConversationWidget.generated.h"

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
	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	TObjectPtr<class UTextBlock> Txt_NPCName;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Conversation;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<class UImage> Img_Portrait;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Exit;

public:
	UFUNCTION()
	void OnClicked_Exit();

	void UpdateConversationUI(const FString& NPCName, const FString& NewConversation);
	
};
