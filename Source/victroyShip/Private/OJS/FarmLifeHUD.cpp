// Fill out your copyright notice in the Description page of Project Settings.


#include "OJS/FarmLifeHUD.h"
#include "OJS/MainMenuWidget.h"
#include "OJS/InteractionWidget.h"

AFarmLifeHUD::AFarmLifeHUD()
{
}

void AFarmLifeHUD::BeginPlay()
{
	Super::BeginPlay();

	if(MainMenuClass)
	{
		MainMenuWidget = CreateWidget<UMainMenuWidget>(GetWorld(), MainMenuClass);
		MainMenuWidget->AddToViewport(5);
		MainMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
	
	if(InteractionWidgetClass)
	{
		InteractionWidget = CreateWidget<UInteractionWidget>(GetWorld(), InteractionWidgetClass);
		InteractionWidget->AddToViewport(-1);
		InteractionWidget->SetVisibility(ESlateVisibility::Collapsed);
	}


	
}

void AFarmLifeHUD::DisplayMenu()
{
	if(MainMenuWidget)
	{
		bIsMenuVisible = true;
		MainMenuWidget->SetVisibility(ESlateVisibility::Visible);
	}	
}

void AFarmLifeHUD::HideMenu()
{
	if(MainMenuWidget)
	{
		bIsMenuVisible = false;
		MainMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	}	
}

void AFarmLifeHUD::ShowInteractionWidget() const
{
	if(InteractionWidget)
	{
		InteractionWidget->SetVisibility(ESlateVisibility::Visible);
	}	
}

void AFarmLifeHUD::HideInteractionWidget() const
{
	if(InteractionWidget)
	{
		InteractionWidget->SetVisibility(ESlateVisibility::Collapsed);
	}	
}

void AFarmLifeHUD::UpdateInteractionWidget(const FInteractableData* InteractableData) const
{
	if(InteractionWidget)
	{
		if(InteractionWidget->GetVisibility() == ESlateVisibility::Collapsed)
		{
			InteractionWidget->SetVisibility(ESlateVisibility::Visible);
		}
		InteractionWidget->UpdateWidget(InteractableData);
	}
}


