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

USTRUCT(BlueprintType, Blueprintable)
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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plant")
    int HarvestAmount;

    bool IsValid() const
    {
	    return !Name.IsEmpty();
    }

    FString GetPath(bool bGrow, int index) const
    {
	    if (bGrow)
	    {
		    switch (index)
		    {
		    case 1: return GrowPath1;
		    case 2: return GrowPath2;
		    case 3: return GrowPath3;
            case 4: return MaturePath;
		    default: return "";
		    }
	    }
        else
        {
            switch (index)
            {
            case 1: return HavestedPath1;
            case 2: return HavestedPath2;
            case 3: return MaturePath;
            default: return "";
            }
        }
    }
};

UCLASS()
class VICTROYSHIP_API UPlantStructure : public UDataTable
{
	GENERATED_BODY()
	
};
