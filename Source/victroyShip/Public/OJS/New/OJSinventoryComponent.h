// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OJS/NEW/OJSitemObject.h"
#include "OJSinventoryComponent.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VICTROYSHIP_API UOJSinventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UOJSinventoryComponent();
	
	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TArray<UOJSitemObject*> InventoryItems;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddItem(UOJSitemObject* Item);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RemoveItem(UOJSitemObject* Item);
};
