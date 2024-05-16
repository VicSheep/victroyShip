// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TextInputComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VICTROYSHIP_API UTextInputComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTextInputComponent();

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UChatUIWidget> ChatUIClass;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UChatUIWidget> ChatUI;

protected:
	bool IsChatting = false;

public:	
	void Chat();
		
};
