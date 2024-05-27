// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlantWidget.generated.h"

/**
 * 
 */
UCLASS()
class VICTROYSHIP_API UPlantWidget : public UUserWidget
{
	GENERATED_BODY()

public:						
	virtual void NativeConstruct() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	class UUniformGridPanel* grid;

	///* Player *///
	UPROPERTY(EditAnywhere, Category = "Target")
	class UFarmerComponent* FarmerComponent;

	///* Create Buttons *///
	UPROPERTY()
	TArray<class UPlantButtonWidget*> Buttons;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UPlantButtonWidget> ButtonFactory;

	UFUNCTION()
	void CreateButtons();

	///* Plant *///
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AGroundActor* Ground;

	UFUNCTION()
	void SetPlantWidget(AGroundActor* gound);
};
