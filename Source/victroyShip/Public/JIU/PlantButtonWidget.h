// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlantButtonWidget.generated.h"

/**
 * 
 */
UCLASS()
class VICTROYSHIP_API UPlantButtonWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	class UButton* btn_plant;

	UPROPERTY()
	class UPlantWidget* panel;

	UPROPERTY()
	int id;

	UFUNCTION()
	void InitValue(UPlantWidget* _panel, int _id);

	UFUNCTION()
	void ClickButton();
};
