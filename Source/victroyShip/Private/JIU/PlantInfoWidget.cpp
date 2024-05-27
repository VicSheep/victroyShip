// Fill out your copyright notice in the Description page of Project Settings.


#include "JIU/PlantInfoWidget.h"

#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "JIU/GroundActor.h"
#include "JIU/PlantActor.h"

void UPlantInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	button_grow->OnClicked.AddDynamic(this, &UPlantInfoWidget::ClickGrowButton);
}

void UPlantInfoWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (this->IsVisible())
	{
		UpdatePlantInfo();
	}
}

void UPlantInfoWidget::SetPlantInfo(APlantActor* plant)
{
	if (plant)
	{
		if (plant->Ground)
		{
			this->ground = plant->Ground;
		}

		if (plant->PlantInfo.IsValid())
		{
			text_type->SetText(FText::FromString(plant->PlantInfo.Name));
		}
	}
}

void UPlantInfoWidget::UpdatePlantInfo()
{
	if (ground)
	{
		bar_water->SetPercent(ground->WaterFigure / 100.f);
		bar_fertilizer->SetPercent(ground->FertilizerFigure / 100.f);
	}
}

void UPlantInfoWidget::ClickGrowButton()
{
	if (ground)
	{
		if (ground->Plant)
		{
			ground->MoveCamera(true);
			// Ground->Plant->GrowPlant();
		}
	}

}
