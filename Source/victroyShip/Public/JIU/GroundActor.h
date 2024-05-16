// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GroundActor.generated.h"

UCLASS()
class VICTROYSHIP_API AGroundActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGroundActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	///* Component *///
	UPROPERTY(EditAnywhere)
	class UBoxComponent* BoxComponent;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* MeshComponent;

	///* Mesh *///
	FString PlanterPath = "/Game/UltimateFarming/Meshes/SM_Planter_D_01.SM_Planter_D_01";

	///* Planting Seed *///
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class APlantActor* Plant;
	
	UFUNCTION(BlueprintCallable)
	void PlantingSeed(int id);

	///* Manage Ground/Plant *///
	UPROPERTY()
	float WaterFigure;

	UPROPERTY()
	float FertilizerFigure;

	UFUNCTION(BlueprintCallable)
	void WaterPlant();

	UFUNCTION(BlueprintCallable)
	void fertilizePlant();
};
