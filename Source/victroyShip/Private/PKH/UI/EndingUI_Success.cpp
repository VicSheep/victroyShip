// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/UI/EndingUI_Success.h"

#include "Kismet/GameplayStatics.h"

void UEndingUI_Success::NativeConstruct()
{
	Super::NativeConstruct();

	OnEndingFinishedDelegate.BindDynamic(this, &UEndingUI_Success::OnEndingFinished);
	BindToAnimationFinished(Anim_Ending_Success, OnEndingFinishedDelegate);

	PlayAnimation(Anim_Ending_Success);

	TitleLevelName = TEXT("");
}

void UEndingUI_Success::OnEndingFinished()
{
	//UGameplayStatics::OpenLevel(GetWorld(), TitleLevelName);
}
