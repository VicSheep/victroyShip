// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PKH/Interface/HourUpdate.h"
#include "PKH/NPC/NPCBase.h"
#include "NPC_Fisherman.generated.h"

/**
 * 
 */
UCLASS()
class VICTROYSHIP_API ANPC_Fisherman : public ANPCBase, public IHourUpdate
{
	GENERATED_BODY()

public:
	ANPC_Fisherman();

protected:
	virtual void BeginPlay() override;

	virtual void DoJob() override;

// Locations
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Locations")
	FVector WorkLoc;

// Interface
public:
	virtual void OnHourUpdated(int32 NewHour) override;

};
