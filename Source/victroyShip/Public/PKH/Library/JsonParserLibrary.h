// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PKH/Game/FarmLifeGameMode.h"
#include "JsonParserLibrary.generated.h"

/**
 * 
 */
UCLASS()
class VICTROYSHIP_API UJsonParserLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static void ParseNPCResponse(const FString& Data, FNPCResponse& Response);
};
