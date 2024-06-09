// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PKH/Interface/HourUpdate.h"
#include "PKH/NPC/NPCBase.h"
#include "NPC_Fisher.generated.h"

/**
 * 
 */
UCLASS()
class VICTROYSHIP_API ANPC_Fisher : public ANPCBase, public IHourUpdate
{
	GENERATED_BODY()
	
public:
	ANPC_Fisher();

protected:
	virtual void BeginPlay() override;

	virtual void DoJob() override;

	// Locations
protected:

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Locations")
	FVector FishLoc;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UAnimMontage> Montage_Fishing;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UAnimMontage> Montage_Drink;

	// Interface
public:
	virtual void OnHourUpdated(int32 NewHour) override;
};