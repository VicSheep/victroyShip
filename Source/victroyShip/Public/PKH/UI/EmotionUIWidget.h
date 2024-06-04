#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EmotionUIWidget.generated.h"

/**
 * 
 */
UCLASS()
class VICTROYSHIP_API UEmotionUIWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UEmotionUIWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

// Widgets
protected:
	UPROPERTY(EditDefaultsOnly, meta =(BindWidget))
	TObjectPtr<class UImage> Img_Bubble;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<class UImage> Img_Emotion;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidgetAnim), Transient)
	TObjectPtr<class UWidgetAnimation> Anim_Emotion;

	UFUNCTION()
	void OnVisible(ESlateVisibility InVisibility);

// Textrues
protected:
	// keys for map
	UPROPERTY()
	FString Emotion_Joy = TEXT("joy");

	UPROPERTY()
	FString Emotion_Surprise = TEXT("surprise");

	UPROPERTY()
	FString Emotion_Sadness = TEXT("sadness");

	UPROPERTY()
	FString Emotion_Anger = TEXT("anger");

	UPROPERTY()
	FString Emotion_Noticed = TEXT("noticed");

	// Textures
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UMaterialInstance> MI_Joy;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UMaterialInstance> MI_Surprise;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UMaterialInstance> MI_Sadness;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UMaterialInstance> MI_Anger;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UMaterialInstance> MI_Noticed;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UMaterialInstance> MI_Indiff;

	// Emotion Map
	UPROPERTY(EditDefaultsOnly)
	TMap<FString, class UMaterialInstance*> EmotionMap;

// For External
public:
	void SetEmotion(const FString& Emotion);

};
