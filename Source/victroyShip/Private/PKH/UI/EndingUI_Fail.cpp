// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/UI/EndingUI_Fail.h"

#include "Kismet/GameplayStatics.h"

void UEndingUI_Fail::NativeConstruct()
{
	Super::NativeConstruct();

	OnEndingFinishedDelegate.BindDynamic(this, &UEndingUI_Fail::OnEndingFinished);
	BindToAnimationFinished(Anim_Ending_Fail, OnEndingFinishedDelegate);

	PlayAnimation(Anim_Ending_Fail);

	TitleLevelName = TEXT("0_Title");
}

void UEndingUI_Fail::OnEndingFinished()
{
	//UGameplayStatics::OpenLevel(GetWorld(), TitleLevelName);
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, false);
}
