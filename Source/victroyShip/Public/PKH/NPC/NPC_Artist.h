// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PKH/Interface/HourUpdate.h"
#include "PKH/NPC/NPCBase.h"
#include "NPC_Artist.generated.h"

/**
 * 
 */
UCLASS()
class VICTROYSHIP_API ANPC_Artist : public ANPCBase, public IHourUpdate
{
	GENERATED_BODY()

public:
	ANPC_Artist();

protected:
	virtual void BeginPlay() override;

	virtual void DoJob() override;

// Locations
protected:
	UPROPERTY(EditAnywhere, Category = "Locations")
	FVector HillLoc;

	UPROPERTY(EditAnywhere, Category = "Locations")
	FVector ParkLoc;

// Interface
public:
	virtual void OnHourUpdated(int32 NewHour) override;
};