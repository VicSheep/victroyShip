// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractionInterface.h"
#include "InteractionWidget.h"
#include "MainMenuWidget.h"
#include "GameFramework/HUD.h"
#include "FarmLifeHUD.generated.h"

class UMainMenuWidget;

UCLASS()
class VICTROYSHIP_API AFarmLifeHUD : public AHUD
{
	GENERATED_BODY()

public:
	//-----------------------------------------------------------------------------
	// PROPERTIES & VARIABLES
	//-----------------------------------------------------------------------------
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UMainMenuWidget> MainMenuClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UInteractionWidget> InteractionWidgetClass;

	bool bIsMenuVisible;
	
	//-----------------------------------------------------------------------------
	// FUNCTIONS
	//-----------------------------------------------------------------------------
	AFarmLifeHUD();

	void DisplayMenu();
	void HideMenu();

	void ShowInteractionWidget();
	void HideInteractionWidget();
	void UpdateInteractionWidget(const FInteractableData* InteractableData);

	
protected:
	//-----------------------------------------------------------------------------
	// PROPERTIES & VARIABLES
	//-----------------------------------------------------------------------------
	UPROPERTY()
	UMainMenuWidget* MainMenuWidget;

	UPROPERTY()
	UInteractionWidget* InteractionWidget;
	
	//-----------------------------------------------------------------------------
	// FUNCTIONS
	//-----------------------------------------------------------------------------
	virtual void BeginPlay() override;
};
