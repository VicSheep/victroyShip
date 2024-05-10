// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FarmLifeGameMode.generated.h"

/**
 * 
 */
UCLASS()
class VICTROYSHIP_API AFarmLifeGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AFarmLifeGameMode();

protected:
	virtual void BeginPlay() override;

// Http
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AHttpActor> HttpActorClass;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class AHttpActor> HttpActor;

// STT
protected:
	FString LatestSpeech;

public:
	void SendSpeech(const FString& SpeechFileName, const FString& SpeechFilePath);

	void SetLatestSpeech(const FString& SpeechText);
	FString& GetLatestSpeech();


// time flow
protected:

};
