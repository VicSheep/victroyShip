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

	button_harvest->OnClicked.AddDynamic(this, &UPlantInfoWidget::ClickHarvestButton);
	button_zoomin->OnClicked.AddDynamic(this, &UPlantInfoWidget::ClickZoomButton);
}

void UPlantInfoWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (this->IsVisible())
	{
		UpdatePlantInfo();

		if (ground)
		{
			if (ground->Plant)
			{
				if (ground->Plant->haveChange)
				{
					UpdatePlantState();
				}
			}
			else
			{
				this->SetVisibility(ESlateVisibility::Hidden);
			}
		}
		else
		{
			this->SetVisibility(ESlateVisibility::Hidden);
		}
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
			UpdatePlantState();
		}
		else
		{
			this->SetVisibility(ESlateVisibility::Hidden);
			ground = nullptr;
		}
	}
}

void UPlantInfoWidget::UpdatePlantInfo()
{
	if (ground)
	{
		bar_water->SetPercent(ground->WaterFigure / 100.f);
		// bar_fertilizer->SetPercent(ground->FertilizerFigure / 100.f);
	}
}

void UPlantInfoWidget::UpdatePlantState()
{
	if (ground->Plant)
	{
		text_type->SetText(FText::FromString(ground->Plant->PlantInfo.Name));

		if (ground->Plant->PlantState == EPlantState::Seed)
		{
			text_level->SetText(FText::FromString(FString::Printf(TEXT("Growing up... %d of %d"), 0, ground->Plant->PlantInfo.GrowLevel)));
			button_harvest->SetVisibility(ESlateVisibility::Hidden);
		}
		else if (ground->Plant->PlantState == EPlantState::Growing)
		{
			text_level->SetText(FText::FromString(FString::Printf(TEXT("Growing up... %d of %d"), ground->Plant->CurLevel, ground->Plant->PlantInfo.GrowLevel)));
			button_harvest->SetVisibility(ESlateVisibility::Hidden);
		}
		else if (ground->Plant->PlantState == EPlantState::Havested)
		{
			text_level->SetText(FText::FromString(FString::Printf(TEXT("Bearing fruit... %d of %d"), ground->Plant->CurLevel, ground->Plant->PlantInfo.HavestLevel)));
			button_harvest->SetVisibility(ESlateVisibility::Hidden);
		}
		else if (ground->Plant->PlantState == EPlantState::Mature)
		{
			text_level->SetText(FText::FromString(FString::Printf(TEXT("Harvest!"))));
			button_harvest->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void UPlantInfoWidget::ClickHarvestButton()
{
	if (ground)
	{
		if (ground->Plant)
		{
			if (ground->Plant->PlantState == EPlantState::Mature)
			{
				// ground->Plant->HavestPlant();
				// button_harvest->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
}

void UPlantInfoWidget::ClickZoomButton()
{
	if (ground)
	{
		ground->MoveCamera();
	}
}

/*void UPlantInfoWidget::ClickGrowButton()
{
	if (ground)
	{
		if (ground->Plant)
		{
			if (ground->Plant->PlantState == EPlantState::Mature)
			{
				ground->Plant->HavestPlant();

				if (!ground->Plant)
				{
					this->SetVisibility(ESlateVisibility::Hidden);
					ground->MoveCamera(false);
					ground = nullptr;
				}
				else
				{
					text_grow->SetText(FText::FromString(TEXT("Grow Up")));
				}
			}
			else
			{
				ground->Plant->GrowPlant();

				if (ground->Plant->PlantState == EPlantState::Mature)
				{
					text_grow->SetText(FText::FromString(TEXT("Havest")));
				}
			}
		}
	}

}*/
