// Fill out your copyright notice in the Description page of Project Settings.


#include "EndAttackNotify.h"
#include "BaseCharacter.h"

void UEndAttackNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	ABaseCharacter* ownerChar = Cast<ABaseCharacter>(MeshComp->GetOwner());
	if (ownerChar == nullptr)
		return;
	ownerChar->EndAttack();
}