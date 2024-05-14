// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "FarmLifeHUD.generated.h"

struct FInteractableData;
class UInteractionWidget;
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

	void ShowInteractionWidget() const;
	void HideInteractionWidget() const;
	void UpdateInteractionWidget(const FInteractableData* InteractableData) const;

	
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
