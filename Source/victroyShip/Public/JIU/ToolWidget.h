// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ToolWidget.generated.h"

/**
 * 
 */
UCLASS()
class VICTROYSHIP_API UToolWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	///* Bind *///
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	class UButton* button_0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	class UButton* button_1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	class UButton* button_2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	class UButton* button_3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	class UButton* button_4;

	///* Player *///
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class ATestCharacter* PP;

	void ClickButton(int index);

	UFUNCTION()
	void Onclick0();
	UFUNCTION()
	void Onclick1();
	UFUNCTION()
	void Onclick2();
	UFUNCTION()
	void Onclick3();
	UFUNCTION()
	void Onclick4();
};
