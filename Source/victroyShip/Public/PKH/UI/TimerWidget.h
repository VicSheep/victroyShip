// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TimerWidget.generated.h"

/**
 * 
 */
UCLASS()
class VICTROYSHIP_API UTimerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UTimerWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	TObjectPtr<class UTextBlock> Txt_Date;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Hour;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<class UImage> Img_Fade;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidgetAnim), Transient)
	TObjectPtr<class UWidgetAnimation> FadeOut;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidgetAnim), Transient)
	TObjectPtr<class UWidgetAnimation> FadeIn;

	UPROPERTY(VisibleAnywhere)
	TMap<int32, FString> DayMap;

public:
	void UpdateTimerUI(int32 Date, int32 Hours, int32 Minutes);

	void StartFadeOutAnim();
	void StartFadeInAnim();

	// Delegate
	FWidgetAnimationDynamicEvent FadeOutFinished;
	FWidgetAnimationDynamicEvent FadeInFinished;

	void BindOnFinished();
};
