// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/Animation/AnimNotify_SitComplete.h"

#include "PKH/NPC/NPCBase.h"

void UAnimNotify_SitComplete::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	ANPCBase* NPC = Cast<ANPCBase>(MeshComp->GetOwner());
	if(NPC)
	{
		const FVector SitLoc = NPC->GetActorLocation() + (NPC->GetActorForwardVector() * NPC->GetSitDist());
		NPC->SetActorLocation(SitLoc);
	}
}
