// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PKH/Interface/HourUpdate.h"
#include "PKH/NPC/NPCBase.h"
#include "NPC_Unemployed.generated.h"

/**
 * 
 */
UCLASS()
class VICTROYSHIP_API ANPC_Unemployed : public ANPCBase, public IHourUpdate
{
	GENERATED_BODY()

public:
	ANPC_Unemployed();

// Interface
public:
	virtual void OnHourUpdated(int32 NewHour) override;
};
