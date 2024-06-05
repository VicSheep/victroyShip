// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FarmerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VICTROYSHIP_API UFarmerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFarmerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	///* UI *///
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UPlantWidget> plantFactory;

	UPROPERTY(BlueprintReadOnly)
	class UPlantWidget* PlantWidget;

	void OpenPlantWidget();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UPlantInfoWidget> plantInfoFactory;

	UPROPERTY(BlueprintReadOnly)
	class UPlantInfoWidget* PlantInfoWidget;

	UFUNCTION(BlueprintCallable)
	void OpenInfoWidget();

	UFUNCTION(BlueprintCallable)
	void CloseInfoWidget();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UToolWidget> toolFactory;

	UPROPERTY(BlueprintReadOnly)
	class UToolWidget* ToolWidget;

	UFUNCTION(BlueprintCallable)
	void OpenToolWidget();

	///* Interaction *///
	UFUNCTION(BlueprintCallable)
	void FarmingInteraction();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AGroundActor* Ground;

	///* Tool *///
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurrentTool = 0;

	UFUNCTION()
	void SwitchTool(int index);

	UFUNCTION(BlueprintCallable)
	void HarvestPlant();

	UFUNCTION(BlueprintCallable)
	void RemovePlant();
};
