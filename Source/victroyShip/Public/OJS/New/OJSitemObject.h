// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "OJSitemObject.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class VICTROYSHIP_API UOJSitemObject : public UObject
{
	GENERATED_BODY()

public : 
	UOJSitemObject();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UTexture2D* ItemIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 ItemQuantity;
};
