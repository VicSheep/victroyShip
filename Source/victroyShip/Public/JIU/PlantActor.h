// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlantActor.generated.h"

UENUM()
enum class EPlantType
{
	Grape,
	Sunflower,
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

	///* Component *///
	UPROPERTY(EditAnywhere)
	class UBoxComponent* BoxComponent;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* MeshComponent;

	///* Plant Seed *///
	UFUNCTION()
	void SetPlant(int id, AGroundActor* _ground);

	UPROPERTY()
	EPlantType PlantType;

	UPROPERTY()
	class AGroundActor* Ground;

	///* Growth *///
	UFUNCTION(BlueprintCallable)
	void GrowPlant();

	UPROPERTY()
	int state;

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
