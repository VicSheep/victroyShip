// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PKH/Interface/HourUpdate.h"
#include "PKH/NPC/NPCBase.h"
#include "NPC_Neet.generated.h"

/**
 * 
 */
UCLASS()
class VICTROYSHIP_API ANPC_Neet : public ANPCBase, public IHourUpdate
{
	GENERATED_BODY()
	
public:
	ANPC_Neet();

protected:
	virtual void BeginPlay() override;

	virtual void DoJob() override;

	// Locations
protected:

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Locations")
	FVector NeetSleepingLoc;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Locations")
	FVector NeetSittingLoc;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Locations")
	FVector NeetDancingLoc;

	UPROPERTY(EditDefaultsOnly)
	FRotator NeetSleepingRot;

	UPROPERTY(EditDefaultsOnly)
	FRotator NeetSittingRot;
	
	UPROPERTY(EditDefaultsOnly)
	FRotator NeetDancingRot;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UAnimMontage> Montage_Sleep;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UAnimMontage> Montage_Sit;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UAnimMontage> Montage_Dance;
	// Interface
public:
	virtual void OnHourUpdated(int32 NewHour) override;
};