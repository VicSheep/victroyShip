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
			this->Ground = plant->Ground;
		}

		// Name
		/*switch (plant->PlantType)
		{
		case EPlantType::Grape:
			text_type->SetText(FText::FromString("GRAPE"));
			break;
		case EPlantType::Sunflower:
			text_type->SetText(FText::FromString("SUNFLOWER"));
			break;
		default:
			text_type->SetText(FText::FromString("ERROR"));
		}*/
	}
}

void UPlantInfoWidget::UpdatePlantInfo()
{
	if (Ground)
	{
		bar_water->SetPercent(Ground->WaterFigure / 100.f);
		bar_fertilizer->SetPercent(Ground->FertilizerFigure / 100.f);
	}
}

void UPlantInfoWidget::ClickGrowButton()
{
	if (Ground)
	{
		if (Ground->Plant)
		{
			Ground->Plant->GrowPlant();
		}
	}

}
