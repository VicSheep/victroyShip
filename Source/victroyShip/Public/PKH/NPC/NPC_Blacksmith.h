// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

protected:
	UPROPERTY(EditAnywhere)
	FVector HomeLoc = FVector(720, -330, 88);

	UPROPERTY(EditAnywhere)
	FVector TargetLoc = FVector(-220, -330, 88);

// Interface
public:
	virtual void OnHourUpdated(int32 NewHour) override;

};
