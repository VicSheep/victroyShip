// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GroundActor.generated.h"

class UNiagaraSystem;

UENUM(BlueprintType)
enum class EGroundState : uint8
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

	UPROPERTY(EditAnywhere)
	class UCameraComponent* CameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UChildActorComponent* ActorComponent;

	///* Mesh or Material *///
	UPROPERTY()
	UMaterialInterface* DefaultMaterialInterface;

	UPROPERTY()
	UMaterialInterface* DryMaterialInterface;

	UPROPERTY()
	UMaterialInterface* MiddleMaterialInterface;

	UPROPERTY()
	UMaterialInterface* WetMaterialInterface;

	FString PlanterPath = "/Game/UltimateFarming/Meshes/SM_Planter_D_02.SM_Planter_D_02";
	FString DefaultMaterialPath = "/Game/UltimateFarming/Materials/MI_FertileGround.MI_FertileGround";
	FString DryMaterialPath = "/Game/JIU/Materials/MI_Planter_Dry.MI_Planter_Dry";
	FString MiddleMaterialPath = "/Game/JIU/Materials/MI_Planter_Middle.MI_Planter_Middle";
	FString WetMaterialPath = "/Game/JIU/Materials/MI_Planter_Wet.MI_Planter_Wet";

	///* Player *///
	UPROPERTY()
	class APlayerController* PC;

	UPROPERTY()
	class APawn* PP;

	UPROPERTY()
	class UPrimitiveComponent* PrimitiveComponent;

	///* Variable *///
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
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
	void RemovePlant();

	UFUNCTION(BlueprintCallable)
	void ProwGround();

	///* Weed *///
	UPROPERTY()
	class AWeedActor* WeedActor;

	int32 RandomNumber = 0;
	int32 Cushion = 0;
	bool isWeed = false;

	UFUNCTION(BlueprintCallable)
	void RemoveWeed();

	///* Camera Blend *///
	UFUNCTION()
	void MoveCamera();

	UPROPERTY(BlueprintReadWrite)
	bool isZoom = false;

	///* Others *///
	UFUNCTION()
	void SetGroundMaterial();

	void SetGroundState(EGroundState state);

	///* Particle *///
	UPROPERTY()
	class UNiagaraComponent* NiagaraComponent;

	UPROPERTY()
	UNiagaraSystem* DustNiagaraSystem;

	UPROPERTY()
	UNiagaraSystem* RainNiagaraSystem;

	UPROPERTY()
	UNiagaraSystem* LeafNiagaraSystem;

	UFUNCTION()
	void SpawnNiagaraSystem(UNiagaraSystem* niagara);

	///* Widget *///
	bool haveChange = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UPlantInfoWidget> WidgetClass;

	UPROPERTY()
	TArray<UUserWidget*> FoundWidgets;

	///* Sound *///
	UFUNCTION()
	void PlaySound(USoundWave* sound);
	
	UPROPERTY()
	USoundWave* HoeSoundWave;

	UPROPERTY()
	USoundWave* WaterSoundWave;

	UPROPERTY()
	USoundWave* SandSoundWave;

	UPROPERTY()
	USoundWave* AxeSoundWave;
};
