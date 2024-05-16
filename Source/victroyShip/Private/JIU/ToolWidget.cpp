// Fill out your copyright notice in the Description page of Project Settings.


#include "JIU/ToolWidget.h"

#include "Components/Button.h"
#include "JIU/TestCharacter.h"

void UToolWidget::NativeConstruct()
{
	Super::NativeConstruct();

	APlayerController* pc = GetWorld()->GetFirstPlayerController();

    if (pc)
    {
        PP = Cast<ATestCharacter>(pc->GetPawn());
    }

    button_0->OnClicked.AddDynamic(this, &UToolWidget::Onclick0);
    button_1->OnClicked.AddDynamic(this, &UToolWidget::Onclick1);
    button_2->OnClicked.AddDynamic(this, &UToolWidget::Onclick2);
    button_3->OnClicked.AddDynamic(this, &UToolWidget::Onclick3);
    button_4->OnClicked.AddDynamic(this, &UToolWidget::Onclick4);

}

void UToolWidget::ClickButton(int index)
{
    if (PP)
    {
	    PP->SwitchTool(index);
    }
}

void UToolWidget::Onclick0()
{
    ClickButton(0);
}

void UToolWidget::Onclick1()
{
    ClickButton(1);
}

void UToolWidget::Onclick2()
{
    ClickButton(2);
}

void UToolWidget::Onclick3()
{
    ClickButton(3);
}

void UToolWidget::Onclick4()
{
    ClickButton(4);
}
