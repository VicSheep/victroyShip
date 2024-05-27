// Fill out your copyright notice in the Description page of Project Settings.


#include "JIU/PlantWidget.h"

#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "JIU/PlantButtonWidget.h"

void UPlantWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CreateButtons();
}

void UPlantWidget::CreateButtons()
{
	Buttons.SetNum(16);

	for (int i = 0; i < 16; i++)
	{
		int row = i / 4;
		int col = i % 4;

		Buttons[i] = CreateWidget<UPlantButtonWidget>(GetWorld(), ButtonFactory);
		Buttons[i]->InitValue(this, i);

		auto slot = grid->AddChildToUniformGrid(Buttons[i], row, col);
		slot->SetHorizontalAlignment(HAlign_Fill);
		slot->SetVerticalAlignment(VAlign_Fill);
	}
}
