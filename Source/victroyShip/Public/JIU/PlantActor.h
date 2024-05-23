// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlantStructure.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "PlantActor.generated.h"

class AGroundActor;

UENUM()
enum class EPlantState
{
	Seed,
	Growing,
	Mature,
	Havested,
};

UCLASS()
class VICTROYSHIP_API APlantActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlantActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Destroyed() override;

	int stateInt;

	///* Component *///
	UPROPERTY(EditAnywhere)
	class UBoxComponent* BoxComponent;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* MeshComponent;

	///* Data Table *///
	UPROPERTY(EditDefaultsOnly, Category = "Data")
	UDataTable* PlantDataTable;

	UFUNCTION()
	FPlantStruct GetPlantData(FName RowName);

	FString PlantDataTablePath = "/Game/JIU/Others/Datatable_Plant.Datatable_Plant";

	///* Plant *///
	UPROPERTY()
	FPlantStruct PlantInfo;

	UPROPERTY()
	EPlantState PlantState;

	bool isRepeated = false;

	int MaxGrowLevel = -1;
	int MaxHavestLevel = -1;
	int CurLevel = 0;

	///* Plant Seed *///
	UFUNCTION()
	void SetPlant(int id, AGroundActor* _ground);

	UPROPERTY()
	class AGroundActor* Ground;

	///* Growth *///
	UFUNCTION(BlueprintCallable)
	void GrowPlant();

	UFUNCTION()
	void StartScaling();

	UFUNCTION()
	void HandleProgress(float Value);

	UFUNCTION()
	void OnTimelineFinished();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Timeline")
	UCurveFloat* FloatCurve;

	FTimeline MyTimeline;

	FVector InitialScale = FVector(1.0f, 1.0f, 1.0f);
	FVector MaxScale = FVector(1.0f, 1.0f, 1.0f);

	UPROPERTY()
	UStaticMesh* NewMesh;
	bool isChanged = true;

	void SetupTimeline();

	///* Havest *///
	UFUNCTION(BlueprintCallable)
	void HavestPlant();

	///* Mesh pathes *///
	FString GrapePath0 = "/Game/UltimateFarming/Meshes/SM_BambooHatch_B.SM_BambooHatch_B";
	FString GrapePath1 = "/Game/UltimateFarming/Meshes/SM_Grape_Starter.SM_Grape_Starter";
	FString GrapePath2 = "/Game/UltimateFarming/Meshes/SM_Grape_C_Harvested.SM_Grape_C_Harvested";
	FString GrapePath3 = "/Game/UltimateFarming/Meshes/SM_Grape_A_Harvested.SM_Grape_A_Harvested"; // Havested
	FString GrapePath4 = "/Game/UltimateFarming/Meshes/SM_Grape_A.SM_Grape_A";

	FString SunflowerPath1 = "/Game/UltimateFarming/Meshes/SM_Sunflower_Starter.SM_Sunflower_Starter";
	FString SunflowerPath2 = "/Game/UltimateFarming/Meshes/SM_Sunflower_C.SM_Sunflower_C";
	FString SunflowerPath3 = "/Game/UltimateFarming/Meshes/SM_Sunflower_A.SM_Sunflower_A";
};
