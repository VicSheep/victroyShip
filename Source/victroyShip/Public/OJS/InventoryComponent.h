// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnInventoryUpdated);

class UItemBase;

UENUM(BlueprintType)
enum class EItemAddResult : uint8
{
	IAR_NoItemAdded UMETA(DisplaayName = "No item added"),
	IAR_PartialAmountItemAdded UMETA(DisplaayName = "Partial amount of item added"),
	IAR_AllItemAdded UMETA(DisplaayName = "All of item added")
};

USTRUCT(BlueprintType)
struct FItemAddResult
{
	GENERATED_BODY()

	FItemAddResult() :
	ActualAmountAdded(0),
	OperationResult(EItemAddResult::IAR_NoItemAdded),
	ResultMessage(FText::GetEmpty())
	{
		
	};
	
	// Actual amount of item that was added to the inventory
	UPROPERTY(BlueprintReadOnly, Category = "Item Add Result")
	int32 ActualAmountAdded;
	// Enum representing the end state of and item operation
	UPROPERTY(BlueprintReadOnly, Category = "Item Add Result")
	EItemAddResult OperationResult;
	// Informational message that can be passed with the result
	UPROPERTY(BlueprintReadOnly, Category = "Item Add Result")
	FText ResultMessage;
	
	static FItemAddResult AddedNone(const FText& ErrorText)
	{
		FItemAddResult AddedNoneResult;
		AddedNoneResult.ActualAmountAdded = 0;
		AddedNoneResult.OperationResult = EItemAddResult::IAR_NoItemAdded;
		AddedNoneResult.ResultMessage = ErrorText;
		return AddedNoneResult;
	};
	static FItemAddResult AddedPartial(const int32 PartialAmountAdded, const FText& ErrorText)
	{
		FItemAddResult AddedPartialResult;
		AddedPartialResult.ActualAmountAdded = PartialAmountAdded;
		AddedPartialResult.OperationResult = EItemAddResult::IAR_PartialAmountItemAdded;
		AddedPartialResult.ResultMessage = ErrorText;
		return AddedPartialResult;
	};
	static FItemAddResult AddedAll(const int32 AmountAdded, const FText& Message)
	{
		FItemAddResult AddedAllResult;
		AddedAllResult.ActualAmountAdded = AmountAdded;
		AddedAllResult.OperationResult = EItemAddResult::IAR_AllItemAdded;
		AddedAllResult.ResultMessage = Message;
		return AddedAllResult;
	};
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VICTROYSHIP_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	//-----------------------------------------------------------------------------
	// PROPERTIES & VARIABLES
	//-----------------------------------------------------------------------------
	FOnInventoryUpdated OnInventoryUpdated;
	
	//-----------------------------------------------------------------------------
	// FUNCTIONS
	//-----------------------------------------------------------------------------
	UInventoryComponent();

	//UFUNCTION(Category = "Inventory")
	//FItemAddResult HandleAddItem(UItemBase* InputItem);

	UFUNCTION(Category = "Inventory")
	UItemBase* FindMatchingItem(UItemBase* ItemIn) const;
	UFUNCTION(Category = "Inventory")
	UItemBase* FindNextItemByID(UItemBase* ITemIn) const;
	UFUNCTION(Category = "Inventory")
	UItemBase* FindNextPartialStack(UItemBase* ItemIn) const;

//	UFUNCTION(Category = "Inventory")
//	void RemoveSingleInstanceOfItem(UItemBase* ItemIn);
	UFUNCTION(Category = "Inventory")
	int32 RemoveAmountOfItem(UItemBase* ItemIn, int32 DesiredAmountToRemove);
//	UFUNCTION(Category = "Inventory")
//	void SplitExistingStack(UItemBase* ItemIn, const int32 AmountToSplit);

	// getters
	UFUNCTION(Category = "Inventory")
	FORCEINLINE float GetInventoryTotalWeight() const {return InventoryTotalWeight;};
	UFUNCTION(Category = "Inventory")
	FORCEINLINE float GetWeightCapacity() const {return InventoryWeightCapacity;};
	UFUNCTION(Category = "Inventory")
	FORCEINLINE int32 GetSlotsCapacity() const {return InventorySlotsCapacity;};
	UFUNCTION(Category = "Inventory")
	FORCEINLINE TArray<UItemBase*> GetInventoryContents() const {return InventoryContents;};
	//setters
	UFUNCTION(Category = "Inventory")
	FORCEINLINE void SetSlotsCapacity(const int32 NewSlotsCapacity) {InventorySlotsCapacity = NewSlotsCapacity;};
	UFUNCTION(Category = "Inventory")
	FORCEINLINE void SetWeightCapacity(const float NewWeightCapacity) {InventoryWeightCapacity = NewWeightCapacity;};
 
protected:
	//-----------------------------------------------------------------------------
	// PROPERTIES & VARIABLES
	//-----------------------------------------------------------------------------
	UPROPERTY(VisibleAnywhere, Category = "INventory")
	float InventoryTotalWeight;
	UPROPERTY(EditInstanceOnly, Category = "INventory")
	int32 InventorySlotsCapacity;
	UPROPERTY(EditInstanceOnly, Category = "INventory")
	float InventoryWeightCapacity;

	UPROPERTY(VisibleAnywhere, Category = "INventory")
	TArray<TObjectPtr<UItemBase>> InventoryContents;
	
	//-----------------------------------------------------------------------------
	// FUNCTIONS
	//-----------------------------------------------------------------------------
	virtual void BeginPlay() override;

//	FItemAddResult HandleNonStackableItems(UItemBase*, int32 RequestedAddAmount);
	int32 HandleStackableItems(UItemBase*, int32 RequestedAddAmount);
	int32 CalculateWeightAddAmount(UItemBase*, int32 RequestedAddAmount);
	int32 CalculateNumberForFullStack(UItemBase* ExistingItem, int32 InitialRequestedAddAmount);

//	void AddNewItem(UItemBase* Item, const int32 AmountToAdd);
};
