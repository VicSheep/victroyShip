// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatUIWidget.generated.h"

/**
 * 
 */
UCLASS()
class VICTROYSHIP_API UChatUIWidget : public UUserWidget
{
	GENERATED_BODY()


protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	TObjectPtr<class UEditableText> Editable_Chat;

public:
	void Focus();
	FString GetChatText() const;

	FORCEINLINE TObjectPtr<class UEditableText> GetChatWidget() const {	return Editable_Chat; };
};
