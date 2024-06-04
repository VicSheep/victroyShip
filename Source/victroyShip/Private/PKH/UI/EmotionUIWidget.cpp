// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/UI/EmotionUIWidget.h"

#include "Components/Image.h"

UEmotionUIWidget::UEmotionUIWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> MI_JoyRef(TEXT("/Script/Engine.MaterialInstanceConstant'/Game/PKH/Material/MI_Joy.MI_Joy'"));
	if(MI_JoyRef.Object)
	{
		MI_Joy = MI_JoyRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> MI_SurpriseRef(TEXT("/Script/Engine.MaterialInstanceConstant'/Game/PKH/Material/MI_Surprise.MI_Surprise'"));
	if (MI_SurpriseRef.Object)
	{
		MI_Surprise = MI_SurpriseRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> MI_SadnessRef(TEXT("/Script/Engine.MaterialInstanceConstant'/Game/PKH/Material/MI_Sad.MI_Sad'"));
	if (MI_SadnessRef.Object)
	{
		MI_Sadness = MI_SadnessRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> MI_AngerRef(TEXT("/Script/Engine.MaterialInstanceConstant'/Game/PKH/Material/MI_Angry.MI_Angry'"));
	if (MI_AngerRef.Object)
	{
		MI_Anger = MI_AngerRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> MI_NoticedRef(TEXT("/Script/Engine.MaterialInstanceConstant'/Game/PKH/Material/MI_Noticed.MI_Noticed'"));
	if (MI_NoticedRef.Object)
	{
		MI_Noticed = MI_NoticedRef.Object;
	}

	// Emotion which is not in map
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> MI_IndiffRef(TEXT("/Script/Engine.MaterialInstanceConstant'/Game/PKH/Material/MI_Indiff.MI_Indiff'"));
	if (MI_IndiffRef.Object)
	{
		MI_Indiff = MI_IndiffRef.Object;
	}

	EmotionMap.Add(Emotion_Joy, MI_Joy);
	EmotionMap.Add(Emotion_Surprise, MI_Surprise);
	EmotionMap.Add(Emotion_Sadness, MI_Sadness);
	EmotionMap.Add(Emotion_Anger, MI_Anger);
	EmotionMap.Add(Emotion_Noticed, MI_Noticed);
}

void UEmotionUIWidget::NativeConstruct()
{
	Super::NativeConstruct();

	OnVisibilityChanged.AddDynamic(this, &UEmotionUIWidget::OnVisible);
}

void UEmotionUIWidget::OnVisible(ESlateVisibility InVisibility)
{
	if(InVisibility == ESlateVisibility::Visible)
	{
		PlayAnimation(Anim_Emotion);
	}
}

void UEmotionUIWidget::SetEmotion(const FString& Emotion)
{
	if(EmotionMap.Contains(Emotion))
	{
		Img_Emotion->SetBrushFromMaterial(EmotionMap[Emotion]);
	}
	else
	{
		Img_Emotion->SetBrushFromMaterial(MI_Indiff);
	}
}
