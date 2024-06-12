// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/Animation/AnimNotify_StartSit.h"

#include "PKH/NPC/NPCBase.h"

void UAnimNotify_StartSit::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	ANPCBase* NPC = Cast<ANPCBase>(MeshComp->GetOwner());
	if (NPC)
	{
		NPC->StartSit();
	}
}
