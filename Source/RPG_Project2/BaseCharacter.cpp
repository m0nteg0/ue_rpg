// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Math/UnrealMathUtility.h"
#include "Engine/World.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/CapsuleComponent.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateLookAt(DeltaTime);
	UpdateMoveTo(DeltaTime);
}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABaseCharacter::MoveForward(float axis_value, float start_acc, float stop_acc)
{
	float min_value = 0.000001f;
	if (fabs(axis_value) < min_value && fabs(ForwardScaleValue) < min_value)
		return;

	FRotator rotation = GetActorRotation();
	rotation = FRotator(0, rotation.Yaw, 0);
	FVector forwardDir = FRotationMatrix(rotation).GetScaledAxis(EAxis::X);

	if (fabs(axis_value) > min_value)
	{
		float delta_time = GetWorld()->GetDeltaSeconds();
		ForwardScaleValue = FMath::FInterpTo(ForwardScaleValue, axis_value, delta_time, start_acc);

		AddMovementInput(forwardDir, ForwardScaleValue);
	}
	else if (ForwardScaleValue > fabs(0.3))
	{
		float delta_time = GetWorld()->GetDeltaSeconds();
		ForwardScaleValue = FMath::FInterpTo(ForwardScaleValue, 0, delta_time, stop_acc);

		AddMovementInput(forwardDir, ForwardScaleValue);
	}
	else
		ForwardScaleValue = 0;
}

void ABaseCharacter::MoveRight(float axis_value, float start_acc, float stop_acc)
{
	float min_value = 0.000001f;
	if (fabs(axis_value) < min_value && fabs(RightScaleValue) < min_value)
		return;

	FRotator rotation = GetActorRotation();
	rotation = FRotator(0, rotation.Yaw, 0);
	FVector rightDir = FRotationMatrix(rotation).GetScaledAxis(EAxis::Y);

	if (fabs(axis_value) > min_value)
	{
		float delta_time = GetWorld()->GetDeltaSeconds();
		RightScaleValue = FMath::FInterpTo(RightScaleValue, axis_value, delta_time, start_acc);

		AddMovementInput(rightDir, RightScaleValue);
	}
	else if (RightScaleValue > fabs(0.3))
	{
		float delta_time = GetWorld()->GetDeltaSeconds();
		RightScaleValue = FMath::FInterpTo(RightScaleValue, 0, delta_time, stop_acc);

		AddMovementInput(rightDir, RightScaleValue);
	}
	else
		RightScaleValue = 0;
}

void ABaseCharacter::MoveToLocation(FVector loc, float stop_dist, float start_acc, float stop_acc)
{
	MoveTarget = loc;
	IsMoveTo = true;
	StopDistance = stop_dist;
	StartAcceleration = start_acc;
	StopAcceleration = stop_acc;

	if (StopDistance < GetCapsuleComponent()->GetScaledCapsuleRadius() * 1.5f)
		StopDistance = GetCapsuleComponent()->GetScaledCapsuleRadius() * 1.5f;
}

void ABaseCharacter::MoveToActor(AActor *actor, float stop_dist, float start_acc, float stop_acc)
{
	ActorMoveTarget = actor;
	MoveToLocation(actor->GetActorLocation(), stop_dist, start_acc, stop_acc);
}

void ABaseCharacter::DisableMoveTo()
{
	ForwardScaleValue = 0;
	RightScaleValue = 0;
	ActorMoveTarget = nullptr;
	IsMoveTo = false;
}

bool ABaseCharacter::IsMoveToEnabled()
{
	return IsMoveTo;
}

void ABaseCharacter::LookToLocation(FVector loc)
{
	LookTarget = loc;
	IsLookAt = true;
}

void ABaseCharacter::LookToActor(AActor* actor)
{
	ActorLookTarget = actor;
	LookToLocation(actor->GetActorLocation());
}

void ABaseCharacter::DisableLookAt()
{
	IsLookAt = false;
}

void ABaseCharacter::SetRotationSpeed(float rot_speed)
{
	RotationSpeed = rot_speed;
}

void ABaseCharacter::UpdateMoveTo(float deltaTime)
{
	if (!IsMoveTo)
		return;

	if (ActorMoveTarget != nullptr)
		MoveTarget = ActorMoveTarget->GetActorLocation();

	//Check distance to target location
	float distance = (GetActorLocation() - MoveTarget).Size();
	if (distance < StopDistance * 1.01)
	{
		DisableMoveTo();
		return;
	}

	
	//Get navigation path
	UNavigationPath* path = UNavigationSystemV1::FindPathToLocationSynchronously(
		GetWorld(), GetActorLocation(), MoveTarget, this);

	//Num of path points must be more than 2 or equal 2 (ActorLocation and NextPointLocation)
	if (path->PathPoints.Num() < 2)
		return;

	//Get next point location
	FVector CurrentPoint = path->PathPoints[1];
	
	//Move at point direction
	if (distance < StopDistance + GetCapsuleComponent()->GetScaledCapsuleRadius() * 1.5f)
	{
		DisableLookAt();
		MoveForward(0.0f, StartAcceleration, StopAcceleration);
		if (fabs(ForwardScaleValue) < 0.1)
			IsMoveTo = false;
	}
	else
	{
		//Look at point location
		LookToLocation(CurrentPoint);
		MoveForward(1.0f, StartAcceleration, StopAcceleration);
	}
}

void ABaseCharacter::UpdateLookAt(float deltaTime)
{
	if (!IsLookAt)
		return;

	if (ActorLookTarget != nullptr)
		LookTarget = ActorLookTarget->GetActorLocation();

	FRotator targetRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), LookTarget);
	FRotator new_rot = FMath::RInterpTo(GetActorRotation(), targetRot, deltaTime, RotationSpeed);
	new_rot = FRotator(0, new_rot.Yaw, 0);
	SetActorRotation(new_rot);
}

void ABaseCharacter::HandleActionFromChar(AActor* actor, FString action, 
	bool check_visibility, bool check_audibility)
{
	ABaseCharacter* target = Cast<ABaseCharacter>(actor);
	if (target == nullptr)
		return;

	if (check_visibility)
	{
		if (VisibleTargets.Find(target))
		{
			AAIController* controller = Cast<AAIController>(GetController());
			if (controller != nullptr)
				controller->BrainComponent->GetBlackboardComponent()->SetValueAsBool("ReactToAction", true);
			HandlingTarget = target;
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("React to action! Visible"));
		}
	}
}

void ABaseCharacter::AddVisibleTarget(AActor* actor)
{
	ABaseCharacter *vist_target = Cast<ABaseCharacter>(actor);
	if (vist_target != nullptr)
		VisibleTargets.Add(vist_target);
}

void ABaseCharacter::RemoveVisibleTarget(AActor* actor)
{
	ABaseCharacter* vist_target = Cast<ABaseCharacter>(actor);
	if (vist_target != nullptr)
		VisibleTargets.Remove(vist_target);
}

ABaseCharacter* ABaseCharacter::GetHandlingTarget()
{
	return HandlingTarget;
}

void ABaseCharacter::SetMoveSpeed(float value)
{
	GetCharacterMovement()->MaxWalkSpeed = value;
}
