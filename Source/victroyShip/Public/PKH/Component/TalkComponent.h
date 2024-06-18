// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TalkComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VICTROYSHIP_API UTalkComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTalkComponent();

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class AFarmLifeGameMode> MyGameMode;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class ACharacter> Player;

// Directory
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString RecordFileName = TEXT("Record");

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString RecordFileDir;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString RecordFilePath;

// Search target to do conversation 
protected:
	void SearchNearby(const FString& InputText);

public:
	UFUNCTION(BlueprintCallable)
	void CheckNearbyObjects();

	void CheckNearbyObjects(const FString& InputText);

// Conversation
protected:
	void ConversationWithNPC(class ANPCBase* NewNPC);
	void ConversationWithNPCByText(class ANPCBase* NewNPC, const FString& InputText);

	void TalkToPlant(const TArray<TObjectPtr<class APlantActor>>& NewPlants);
	void TalkToPlantByText(const TArray<TObjectPtr<class APlantActor>>& NewPlants, const FString& InputText);

	bool InConversation = false;

public:
	FORCEINLINE void StartConversation() { InConversation = true; };
	FORCEINLINE void EndConversation() { InConversation = false; };
	FORCEINLINE bool IsInConversation() const { return InConversation; }

// Effect
protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UNiagaraSystem> Vfx_Talk;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UNiagaraComponent> VfxComp;

// UI
public:
	UFUNCTION(BlueprintCallable)
	void RecordOn();

	UFUNCTION(BlueprintCallable)
	void RecordOff();
		
};
