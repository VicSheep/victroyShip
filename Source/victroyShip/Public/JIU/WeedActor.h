// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeedActor.generated.h"

UCLASS()
class VICTROYSHIP_API AWeedActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeedActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* BoxComp;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* MeshComp1;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* MeshComp2;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* MeshComp3;

	FString MeshPath1 = "/Game/UltimateFarming/Meshes/SM_Wheat_A_Harvested.SM_Wheat_A_Harvested";
	FString MeshPath2 = "/Game/UltimateFarming/Meshes/SM_Wheat_B_Harvested.SM_Wheat_B_Harvested";
	FString MeshPath3 = "/Game/UltimateFarming/Meshes/SM_Wheat_C_Harvested.SM_Wheat_C_Harvested";

	void SetVisible(bool isVisible);
};
