// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "NextAttackNotify.generated.h"

/**
 * 
 */
UCLASS()
class RPG_PROJECT2_API UNextAttackNotify : public UAnimNotify
{
	GENERATED_BODY()
	
	void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
