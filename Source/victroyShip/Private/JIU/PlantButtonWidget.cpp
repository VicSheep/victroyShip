// Fill out your copyright notice in the Description page of Project Settings.


#include "JIU/PlantButtonWidget.h"

#include "Components/Button.h"
#include "JIU/GroundActor.h"
#include "JIU/PlantWidget.h"

void UPlantButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	btn_plant->OnClicked.AddDynamic(this, &UPlantButtonWidget::ClickButton);
}

void UPlantButtonWidget::InitValue(UPlantWidget* _panel, int _id)
{
	this->panel = _panel;
	this->id = _id;
}

void UPlantButtonWidget::ClickButton()
{
	if (panel)
	{
		if (panel->ground)
		{
			panel->ground->PlantingSeed(id);
		}

		panel->SetVisibility(ESlateVisibility::Hidden);
	}
}
