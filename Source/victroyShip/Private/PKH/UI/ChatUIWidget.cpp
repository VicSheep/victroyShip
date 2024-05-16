// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/UI/ChatUIWidget.h"

#include "Components/EditableText.h"

void UChatUIWidget::NativeConstruct()
{
	Super::NativeConstruct();


}

void UChatUIWidget::Focus()
{
	Editable_Chat->SetFocus();
}

FString UChatUIWidget::GetChatText() const
{
	FString CurText = Editable_Chat->GetText().ToString();
	Editable_Chat->SetText(FText::FromString(TEXT("")));
	return CurText;
}
