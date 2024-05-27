// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PKH/Interface/DateUpdate.h"
#include "PKH/Interface/HourUpdate.h"
#include "PKH/NPC/NPCBase.h"
#include "NPC_Blacksmith.generated.h"

/**
 * 
 */
UCLASS()
class VICTROYSHIP_API ANPC_Blacksmith : public ANPCBase, public IHourUpdate
{
	GENERATED_BODY()

public:
	ANPC_Blacksmith();

protected:
	virtual void BeginPlay() override;

	virtual void DoJob() override;

// Interface
public:
	virtual void OnHourUpdated(int32 NewHour) override;

};
