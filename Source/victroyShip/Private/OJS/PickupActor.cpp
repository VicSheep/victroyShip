// Fill out your copyright notice in the Description page of Project Settings.


#include "OJS/PickupActor.h"

#include "OJS/ItemBase.h"

// Sets default values
APickupActor::APickupActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>("PickupMesh");
	PickupMesh->SetSimulatePhysics(true);
	SetRootComponent(PickupMesh);

}

// Called when the game starts or when spawned
void APickupActor::BeginPlay()
{
	Super::BeginPlay();

	InitializePickup(UItemBase::StaticClass(), ItemQuantity);
	
}

void APickupActor::InitializePickup(const TSubclassOf<UItemBase> BaseClass, const int32 InQuantity)
{
	if(ItemDataTable && !DesiredItemID.IsNone())
	{
		const FITemData* ItemData = ItemDataTable->FindRow<FITemData>(DesiredItemID, DesiredItemID.ToString());

		ItemReference = NewObject<UItemBase>(this, BaseClass);

		ItemReference->ID = ItemData->ID;
		ItemReference->ItemType = ItemData->ItemType;
		ItemReference->ItemQuality = ItemData->ItemQuality;
		ItemReference->NumericData = ItemData->NumericData;
		ItemReference->TextData = ItemData->TextData;
		ItemReference->AssetData = ItemData->AssetData;

		InQuantity <= 0 ? ItemReference->SetQuantity(1) : ItemReference->SetQuantity(InQuantity);

		PickupMesh->SetStaticMesh(ItemData->AssetData.Mesh);

		UpdateInteractableData();
	}
}

void APickupActor::InitializeDrop(UItemBase* ItemToDrop, const int32 InQuantity)
{
	ItemReference = ItemToDrop;
	InQuantity <= 0 ? ItemReference->SetQuantity(1) : ItemReference->SetQuantity(InQuantity);
	ItemReference->NumericData.Weight = ItemToDrop->GetItemSingleWeight();
	PickupMesh->SetStaticMesh(ItemToDrop->AssetData.Mesh);
	UpdateInteractableData();
}

void APickupActor::UpdateInteractableData()
{
	InstanceInteractableData.InteractableType = EInteractableType::Pickup;
	InstanceInteractableData.Action = ItemReference->TextData.InteractionText;
	InstanceInteractableData.Name = ItemReference->TextData.Name;
	InstanceInteractableData.Quantity = ItemReference->Quantity;
	InteractableData = InstanceInteractableData;
}

void APickupActor::BeginFocus()
{
	if(PickupMesh)
	{
		PickupMesh->SetRenderCustomDepth(true);
	}
}

void APickupActor::EndFocus()
{
	if(PickupMesh)
	{
		PickupMesh->SetRenderCustomDepth(true);
	}
}

void APickupActor::Interact(AFarmLifePlayableCharacter* PlayerCharacter)
{	
	if(PlayerCharacter)
	{
		TakePickup(PlayerCharacter);
	}
}

void APickupActor::TakePickup(const AFarmLifePlayableCharacter* Taker)
{
	if(!IsPendingKillPending())
	{
		if(ItemReference)
		{
			// if(UInventoryComponent* PlayerInventory = Taker->GetInventory());

			//try to add item to player inventory
			// based on result of the add operation
			//abjust or destroy the pickup
			
		}
	}
}



