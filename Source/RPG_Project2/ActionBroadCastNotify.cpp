// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionBroadCastNotify.h"
#include "Kismet/KismetSystemLibrary.h"
#include "BaseCharacter.h"

void UActionBroadCastNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	ABaseCharacter* ownerChar = Cast<ABaseCharacter>(MeshComp->GetOwner());
	if (ownerChar == nullptr)
		return;

	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("Broadcast Action!"));

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
	float radius = 750.0f;
	// Sphere's spawn loccation within the world
	FVector sphereSpawnLocation = ownerChar->GetActorLocation();
	// Class that the sphere should hit against and include in the outActors array (Can be null)
	UClass* seekClass = ABaseCharacter::StaticClass(); // NULL;
	UKismetSystemLibrary::SphereOverlapActors(ownerChar->GetWorld(), sphereSpawnLocation, radius, traceObjectTypes, 0, ignoreActors, outActors);

	// Optional: Use to have a visual representation of the SphereOverlapActors
	//UKismetSystemLibrary::DrawDebugSphere(ownerChar->GetWorld(), sphereSpawnLocation, radius, 12, FColor::Red, true, 10.0f);

	// Finally iterate over the outActor array
	for (AActor* overlappedActor : outActors) {
		ABaseCharacter* overlappedChar = Cast<ABaseCharacter>(overlappedActor);
		if (overlappedChar != nullptr)
			overlappedChar->HandleActionFromChar(ownerChar, "Attack", true, true);
	}
}