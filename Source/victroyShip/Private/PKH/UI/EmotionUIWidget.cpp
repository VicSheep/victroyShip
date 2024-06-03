// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/UI/EmotionUIWidget.h"

#include "Components/Image.h"

UEmotionUIWidget::UEmotionUIWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UTexture2D> Texture_JoyRef(TEXT("/Script/Engine.Texture2D'/Game/PKH/Texture/Joy.Joy'"));
	if(Texture_JoyRef.Object)
	{
		Texture_Joy = Texture_JoyRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UTexture2D> Texture_SurpriseRef(TEXT("/Script/Engine.Texture2D'/Game/PKH/Texture/Surprise.Surprise'"));
	if (Texture_SurpriseRef.Object)
	{
		Texture_Surprise = Texture_SurpriseRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UTexture2D> Texture_SadnessRef(TEXT("/Script/Engine.Texture2D'/Game/PKH/Texture/Sad.Sad'"));
	if (Texture_SadnessRef.Object)                                                
	{
		Texture_Sadness = Texture_SadnessRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UTexture2D> Texture_AngerRef(TEXT("/Script/Engine.Texture2D'/Game/PKH/Texture/Angry.Angry'"));
	if (Texture_AngerRef.Object)
	{
		Texture_Anger = Texture_AngerRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UTexture2D> Texture_IndiffRef(TEXT("/Script/Engine.Texture2D'/Game/PKH/Texture/Indiff.Indiff'"));
	if (Texture_IndiffRef.Object)
	{
		Texture_Indiff = Texture_IndiffRef.Object;
	}

	EmotionMap.Add(Emotion_Joy, Texture_Joy);
	EmotionMap.Add(Emotion_Surprise, Texture_Surprise);
	EmotionMap.Add(Emotion_Sadness, Texture_Sadness);
	EmotionMap.Add(Emotion_Anger, Texture_Anger);
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
		Img_Emotion->SetBrushFromTexture(EmotionMap[Emotion]);
	}
	else
	{
		Img_Emotion->SetBrushFromTexture(Texture_Indiff);
	}
}
