// Fill out your copyright notice in the Description page of Project Settings.


#include "NextAttackNotify.h"
#include "BaseCharacter.h"

void UNextAttackNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	ABaseCharacter* ownerChar = Cast<ABaseCharacter>(MeshComp->GetOwner());
	if (ownerChar == nullptr)
		return;
	if (ownerChar->IsContinueCombo() && ownerChar->getComboCounter() < ownerChar->AttackAnims.Num() )
		ownerChar->NextComboAttack();
}

