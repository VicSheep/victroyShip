// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlantInfoWidget.generated.h"

class APlantActor;
/**
 * 
 */
UCLASS()
class VICTROYSHIP_API UPlantInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	///* UI *///
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* text_type;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	class UProgressBar* bar_water;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	class UProgressBar* bar_fertilizer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	class UButton* button_grow;

	///* Set Info *///
	UFUNCTION(BlueprintCallable)
	void SetPlantInfo(APlantActor* plant);

	class AGroundActor* Ground;

	///* Update Info *///
	UFUNCTION()
	void UpdatePlantInfo();

	///* Click Button *///
	UFUNCTION()
	void ClickGrowButton();
};
