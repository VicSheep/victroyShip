// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EndingUI_Fail.generated.h"

/**
 * 
 */
UCLASS()
class VICTROYSHIP_API UEndingUI_Fail : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnEndingFinished();

// Widgets
protected:
	UPROPERTY(EditDefaultsOnly, meta=(BindWidgetAnim), Transient)
	TObjectPtr<class UWidgetAnimation> Anim_Ending_Fail;

	UPROPERTY(EditDefaultsOnly)
	FName TitleLevelName;

public:
	FWidgetAnimationDynamicEvent OnEndingFinishedDelegate;


};
