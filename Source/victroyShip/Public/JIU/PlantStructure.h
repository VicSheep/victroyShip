// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "PlantStructure.generated.h"

UENUM()
enum class EHavestType
{
    Many,
    Once,
};

USTRUCT(BlueprintType)
struct FPlantStruct : public FTableRowBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plant")
    FString Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plant")
    EHavestType HavestType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plant")
    bool Bamboo;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plant")
    int GrowLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plant")
    int HavestLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plant")
    FString SeedPath;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plant")
    FString GrowPath1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plant")
    FString GrowPath2;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plant")
    FString GrowPath3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plant")
    FString MaturePath;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plant")
    FString HavestedPath1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plant")
    FString HavestedPath2;
};

UCLASS()
class VICTROYSHIP_API UPlantStructure : public UDataTable
{
	GENERATED_BODY()
	
};
