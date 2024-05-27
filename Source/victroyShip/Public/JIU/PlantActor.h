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
};
