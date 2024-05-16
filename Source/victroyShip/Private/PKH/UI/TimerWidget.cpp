// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/UI/TimerWidget.h"

#include "Components/TextBlock.h"

UTimerWidget::UTimerWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	DayMap.Add(1, TEXT("월요일"));
	DayMap.Add(2, TEXT("화요일"));
	DayMap.Add(3, TEXT("수요일"));
	DayMap.Add(4, TEXT("목요일"));
	DayMap.Add(5, TEXT("금요일"));
}

void UTimerWidget::NativeConstruct()
{
	Super::NativeConstruct();


}

void UTimerWidget::UpdateTimerUI(int32 Date, int32 Hours, int32 Minutes)
{
	const FString& DateText = FString::Printf(TEXT("%s %d"), *DayMap[Date], Date);

	const FString& HH = Hours < 10 ? FString::Printf(TEXT("0%d"), Hours) : FString::Printf(TEXT("%d"), Hours);
	const FString& MM = Minutes == 0 ? TEXT("00") : FString::Printf(TEXT("%d"), Minutes);
	const FString& HourText = FString::Printf(TEXT("%s : %s"), *HH, *MM);

	Txt_Date->SetText(FText::FromString(DateText));
	Txt_Hour->SetText(FText::FromString(HourText));
}
