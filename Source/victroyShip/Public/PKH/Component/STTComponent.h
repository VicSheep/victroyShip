// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "STTComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VICTROYSHIP_API USTTComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USTTComponent();

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class AFarmLifeGameMode> MyGameMode;

	UPROPERTY()
	TObjectPtr<class ACharacter> Player;

// Directory
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString SpeechFileName = TEXT("Speech");

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString SpeechFileDir;

public:
	UFUNCTION(BlueprintCallable)
	void CheckNearbyObjects();

protected:
	void ConversationWithNPC(class ANPCBase* NewNPC);
	void TalkToPlant(const TArray<TObjectPtr<AActor>>& NewPlants);

		
};
