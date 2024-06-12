// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PKH/Interface/HourUpdate.h"
#include "PKH/NPC/NPCBase.h"
#include "NPC_Farmer1.generated.h"

/**
 * 
 */
UCLASS()
class VICTROYSHIP_API ANPC_Farmer1 : public ANPCBase, public IHourUpdate
{
	GENERATED_BODY()

public:
	ANPC_Farmer1();

protected:
	virtual void BeginPlay() override;

	virtual void DoJob() override;

	// Locations
protected:

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Locations")
	FVector FarmLoc;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Locations")
	FVector CafeLoc;

	UPROPERTY(EditDefaultsOnly)
	FRotator FarmRot;

	UPROPERTY(EditDefaultsOnly)
	FRotator CafeRot;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UAnimMontage> Montage_FarmWork;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UAnimMontage> Montage_Sit;

	// Interface
public:
	virtual void OnHourUpdated(int32 NewHour) override;
};
