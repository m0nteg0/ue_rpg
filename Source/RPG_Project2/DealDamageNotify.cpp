// Fill out your copyright notice in the Description page of Project Settings.


#include "DealDamageNotify.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/CapsuleComponent.h"
#include "BaseCharacter.h"

void UDealDamageNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	ABaseCharacter* ownerChar = Cast<ABaseCharacter>(MeshComp->GetOwner());
	if (ownerChar == nullptr)
		return;

	// Set what actors to seek out from it's collision channel
	TArray<TEnumAsByte<EObjectTypeQuery>> traceObjectTypes;
	traceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	// Ignore any specific actors
	TArray<AActor*> ignoreActors;
	// Ignore self or remove this line to not ignore any
	ignoreActors.Init(ownerChar, 1);

	// Array of actors that are inside the radius of the sphere
	TArray<AActor*> outActors;

	// Total radius of the sphere
	float radius = 100.0f;
	// Sphere's spawn loccation within the world
	FRotator rotation = ownerChar->GetActorRotation();
	rotation = FRotator(0, rotation.Yaw, 0);
	FVector frowardDir = FRotationMatrix(rotation).GetScaledAxis(EAxis::X);
	FVector sphereSpawnLocation = ownerChar->GetActorLocation() + frowardDir * ownerChar->GetCapsuleComponent()->GetScaledCapsuleRadius() * 2;
	// Class that the sphere should hit against and include in the outActors array (Can be null)
	UClass* seekClass = ABaseCharacter::StaticClass(); // NULL;
	UKismetSystemLibrary::SphereOverlapActors(ownerChar->GetWorld(), sphereSpawnLocation, radius, traceObjectTypes, 0, ignoreActors, outActors);

	// Optional: Use to have a visual representation of the SphereOverlapActors
	//UKismetSystemLibrary::DrawDebugSphere(ownerChar->GetWorld(), sphereSpawnLocation, radius, 12, FColor::Red, true, 10.0f);

	// Finally iterate over the outActor array
	for (AActor* overlappedActor : outActors) {
		ABaseCharacter* overlappedChar = Cast<ABaseCharacter>(overlappedActor);
		if (overlappedChar != nullptr)
		{
			if (overlappedChar->Tags.Num() == 0 || ownerChar->Tags.Num() == 0)
				GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("Character Has not Tag!!!"));

			else if (overlappedChar->Tags[0] != ownerChar->Tags[0])
				overlappedChar->TakeBaseDamage(35);
		}
	}
}