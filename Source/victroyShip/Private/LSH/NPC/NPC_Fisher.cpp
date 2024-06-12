// Fill out your copyright notice in the Description page of Project Settings.


#include "LSH/NPC/NPC_Fisher.h"

#include "PKH/Animation/NPCAnimInstance.h"
#include "PKH/NPC/NPCController.h"
#include "Kismet/GameplayStatics.h"

#define HOUR_FISHING 9
#define HOUR_BACK_HOME 16

ANPC_Fisher::ANPC_Fisher()
{
	NPCType = ENPCType::Fisherman;//임시

	HomeLoc = FVector(1450, -2180, 88);
	FishLoc = FVector(-980, -1410, 88);

	WorkRotation = FRotator(0, 270, 0);

	static ConstructorHelpers::FObjectFinder<UAnimMontage> Montage_WorkRef(TEXT("/Script/Engine.AnimMontage'/Game/PKH/Anim/AM_ArtistWork.AM_ArtistWork'"));
	if (Montage_WorkRef.Object)
	{
		Montage_Work = Montage_WorkRef.Object;
	}

	fishingTool = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("fishingTool"));
	fishingTool->SetupAttachment(GetMesh(), TEXT("middle_01_l")); 
	beerBottle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("beerBottle"));
	beerBottle->SetupAttachment(GetMesh(), TEXT("middle_01_l"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> fishingToolMeshAsset(TEXT("/Game/LSH/props/Stap_za_pecanje.Stap_za_pecanje"));
	if (fishingToolMeshAsset.Succeeded()) fishingTool->SetStaticMesh(fishingToolMeshAsset.Object);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> beerBottleMeshAsset(TEXT("/Game/LSH/props/Beer_Bottle.Beer_Bottle"));
	if (beerBottleMeshAsset.Succeeded()) beerBottle->SetStaticMesh(beerBottleMeshAsset.Object);

	fishingTool->SetVisibility(false);
	fishingTool->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	beerBottle->SetVisibility(false);
	beerBottle->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	fishingTool->SetRelativeScale3D(FVector(0.07));
	fishingTool->SetRelativeLocation(FVector(-8, -2, -66));
	fishingTool->SetRelativeRotation(FRotator(0, -90, 90));
	beerBottle->SetRelativeScale3D(FVector(0.045));
	beerBottle->SetRelativeLocation(FVector(3.539813, -4.180947, -4.08811));
	beerBottle->SetRelativeRotation(FRotator(-204,17,0));
}

void ANPC_Fisher::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("fishing"), FoundActors);
	for (AActor* Actor : FoundActors)
	{
		if (Actor == nullptr)break;
		UE_LOG(LogTemp, Warning, TEXT("Found actor: %s"), *Actor->GetName());
		FishLoc = Actor->GetActorLocation();
		FishRot = Actor->GetActorRotation();
	}
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("FisherHome"), FoundActors);
	for (AActor* Actor : FoundActors)
	{
		if (Actor == nullptr)break;
		UE_LOG(LogTemp, Warning, TEXT("Found actor: %s"), *Actor->GetName());
		FisherHomeLoc = Actor->GetActorLocation();
		FisherHomeRot = Actor->GetActorRotation();
	}
}

void ANPC_Fisher::DoJob()
{
	Super::DoJob();


}

void ANPC_Fisher::OnHourUpdated(int32 NewHour)
{
	if (NewHour == HOUR_FISHING)
	{
		Montage_Work = Montage_Fishing;
		WorkRotation = FishRot;
		NPCController->MoveToTargetLoc(FishLoc);
		NPCController->SetIsWorking(true);
		fishingTool->SetVisibility(true);
		return;
	}

	if (NewHour == HOUR_BACK_HOME)
	{
		AnimInstance->StopSpecificMontage(Montage_Work);
		Montage_Work = Montage_Drink;
		NPCController->MoveToTargetLoc(FisherHomeLoc);
		WorkRotation = FisherHomeRot;
		NPCController->SetIsWorking(true);
		fishingTool->SetVisibility(false);
		beerBottle->SetVisibility(true);
		return;
	}

	if (NewHour == 18)//하루끝나면초기화
	{
		AnimInstance->StopSpecificMontage(Montage_Work);
		beerBottle->SetVisibility(false);
		return;
	}

}