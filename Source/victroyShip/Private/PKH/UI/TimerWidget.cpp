// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/UI/TimerWidget.h"

#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"

UTimerWidget::UTimerWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	DayMap.Add(0, TEXT("월요일"));
	DayMap.Add(1, TEXT("화요일"));
	DayMap.Add(2, TEXT("수요일"));
	DayMap.Add(3, TEXT("목요일"));
	DayMap.Add(4, TEXT("금요일"));
	DayMap.Add(5, TEXT("토요일"));
	DayMap.Add(6, TEXT("일요일"));
}

void UTimerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	
}

void UTimerWidget::UpdateTimerUI(int32 Date, int32 Hours, int32 Minutes)
{
	const FString& DateText = FString::Printf(TEXT("%s %d"), *DayMap[(Date % 7)], Date + 1);

	const FString& HH = Hours < 10 ? FString::Printf(TEXT("0%d"), Hours) : FString::Printf(TEXT("%d"), Hours);
	const FString& MM = Minutes == 0 ? TEXT("00") : FString::Printf(TEXT("%d"), Minutes);
	const FString& HourText = FString::Printf(TEXT("%s : %s"), *HH, *MM);

	Txt_Date->SetText(FText::FromString(DateText));
	Txt_Hour->SetText(FText::FromString(HourText));
}

void UTimerWidget::StartFadeOutAnim()
{
	PlayAnimation(FadeOut, 0, 1);
}

void UTimerWidget::StartFadeInAnim()
{
	PlayAnimation(FadeIn, 0, 1);
}

void UTimerWidget::BindOnFinished()
{
	BindToAnimationFinished(FadeOut, FadeOutFinished);
	BindToAnimationFinished(FadeIn, FadeInFinished);
}

void UTimerWidget::RecordOn()
{
	Img_Record->SetVisibility(ESlateVisibility::Visible);
}

void UTimerWidget::RecordOff()
{
	Img_Record->SetVisibility(ESlateVisibility::Hidden);
}
