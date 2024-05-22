// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GroundActor.generated.h"

UENUM()
enum class EGroundState
{
	Default,
	DryPlanter,
	WetPlanter,
};

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

	///* Mesh or Material *///
	UPROPERTY()
	UMaterialInterface* DefaultMaterialInterface;

	UPROPERTY()
	UMaterialInterface* DryMaterialInterface;

	UPROPERTY()
	UMaterialInterface* WetMaterialInterface;

	///* Path *///
	FString PlanterPath = "/Game/UltimateFarming/Meshes/SM_Planter_D_02.SM_Planter_D_02";
	FString DefaultMaterialPath = "/Game/UltimateFarming/Materials/MI_FertileGround.MI_FertileGround";
	FString DryMaterialPath = "/Game/JIU/Materials/MI_Planter_Dry.MI_Planter_Dry";
	FString WetMaterialPath = "/Game/JIU/Materials/MI_Planter_Wet.MI_Planter_Wet";

	///* Variable *///
	EGroundState GroundState = EGroundState::Default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class APlantActor* Plant;

	UPROPERTY()
	float WaterFigure;

	UPROPERTY()
	float FertilizerFigure;

	float figureLimit = 20.f;

	///* Planting Seed *///
	UFUNCTION(BlueprintCallable)
	void PlantingSeed(int id);

	UPROPERTY(EditAnywhere)
	TSubclassOf<class APlantActor> PlantFactory;

	///* Manage Ground/Plant *///
	UFUNCTION(BlueprintCallable)
	void WaterPlant();

	UFUNCTION(BlueprintCallable)
	void FertilizePlant();

	UFUNCTION(BlueprintCallable)
	void ProwGround();

	UFUNCTION()
	void SetGroundMaterial();
};
