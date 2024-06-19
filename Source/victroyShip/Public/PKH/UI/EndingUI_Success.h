// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EndingUI_Success.generated.h"

/**
 * 
 */
UCLASS()
class VICTROYSHIP_API UEndingUI_Success : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnEndingFinished();

// Widget
protected:
	UPROPERTY(EditDefaultsOnly, meta = (BindWidgetAnim), Transient)
	TObjectPtr<class UWidgetAnimation> Anim_Ending_Success;

	UPROPERTY(EditDefaultsOnly)
	FName TitleLevelName;

public:
	FWidgetAnimationDynamicEvent OnEndingFinishedDelegate;
};
