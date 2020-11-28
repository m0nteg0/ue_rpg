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

#include <ctime>

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
	srand((unsigned)time(0));
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

	if (CharState == State_Hit)
		return;

	if (CharState == State_Idle)
		StopAnimMontage();

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

	if (CharState == State_Hit)
		return;

	if (CharState == State_Idle)
		StopAnimMontage();

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
			float angle_thresh = 70 * (PI / 180.0);

			FVector targ_to_forward = FRotationMatrix(target->GetActorRotation()).GetScaledAxis(EAxis::X);
			FVector targ_to_owner = GetActorLocation() - target->GetActorLocation();
			targ_to_owner.Normalize();

			float angle = FVector::DotProduct(targ_to_forward, targ_to_owner);
			angle = FMath::Acos(angle);

			if (angle > angle_thresh)
				return;

			//srand((unsigned)time(0));
			int randomNumber = rand() % 100;

			int chanceReact = ChanceReactIdle;
			if (CharState == State_Attack)
				chanceReact = ChanceReactInAttack;

			/*GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString::FromInt(randomNumber));
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString::FromInt(chanceReact));*/

			if (MissedReactCounter <= (150 / chanceReact) && randomNumber > chanceReact)
			{
				MissedReactCounter++;
				return;
			}

			MissedReactCounter = 0;

			AAIController* controller = Cast<AAIController>(GetController());
			if (controller != nullptr)
				controller->BrainComponent->GetBlackboardComponent()->SetValueAsBool("ReactToAction", true);
			HandlingTarget = target;
			//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("React to action! Visible"));
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

void ABaseCharacter::TakeBaseDamage(int damage)
{
	if (HitAnims.Num() == 0 || CharState == State_Dodge)
		return;

	if (CharState == State_Block)
	{
		AttackBlocked++;
		return;
	}

	CharState = State_Hit;
	CurrentTakenHit++;
	AttackBlocked++;


	int anim_idx = rand() % HitAnims.Num();
	UAnimMontage* anim = HitAnims[anim_idx];

	PlayAnimMontage(anim);

	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("Taked Damage! "));
}

void ABaseCharacter::BaseAttack()
{
	if (AttackAnims.Num() == 0 || CharState == State_Hit)
		return;

	AttackBlocked = 0;
	CurrentTakenHit = 0;


	if (CharState != State_Attack)
	{
		CharState = State_Attack;
		UAnimMontage* anim = AttackAnims[ComboCounter];
		PlayAnimMontage(anim);
	}
	else if (CharState == State_Attack && !ContinueCombo)
	{
		ContinueCombo = true;
		ComboCounter++;
	}	
}

void ABaseCharacter::BaseContrAttack()
{
	if (ContrAttacksAnims.Num() == 0 || CharState == State_Hit)
		return;

	AttackBlocked = 0;
	CurrentTakenHit = 0;

	int anim_idx = rand() % ContrAttacksAnims.Num();
	UAnimMontage* anim = ContrAttacksAnims[anim_idx];
	PlayAnimMontage(anim);
}

void ABaseCharacter::NextComboAttack()
{
	CharState = State_Idle;
	ContinueCombo = false;
	BaseAttack();
}

void ABaseCharacter::EndAttack()
{
	CharState = State_Idle;
	ContinueCombo = false;
	ComboCounter = 0;

	AAIController* controller = Cast<AAIController>(GetController());
	if (controller != nullptr && ActorLookTarget != nullptr)
		SetRotationSpeed(2.0f);
}

void ABaseCharacter::RandomAttack()
{
	if (AttackAnims.Num() == 0 || CharState == State_Hit)
		return;

	SetRotationSpeed(1.0f);
	CharState = State_Attack;
	int anim_idx = rand() % AttackAnims.Num();
	UAnimMontage* anim = AttackAnims[anim_idx];
	PlayAnimMontage(anim);
}

void ABaseCharacter::BaseBlock()
{
	if (BlockAnims.Num() == 0 || CharState == State_Block)
		return;

	if (CharState == State_Hit)
	{
		if (CurrentTakenHit < MaxTakenHit)
			return;
		else
			CurrentTakenHit = 0;
	}

	CharState = State_Block;
	int anim_idx = rand() % BlockAnims.Num();
	UAnimMontage* anim = BlockAnims[anim_idx];
	PlayAnimMontage(anim);
}

void ABaseCharacter::BaseDodge()
{
	if (DodgeAnims.Num() == 0 || CharState == State_Dodge)
		return;

	if (CharState == State_Hit)
	{
		if (CurrentTakenHit < MaxTakenHit)
			return;
		else
			CurrentTakenHit = 0;
	}

	EndAttack();

	if (RightScaleValue < -0.1)
	{
		CharState = State_Dodge;
		UAnimMontage* anim = DodgeAnims[0];
		PlayAnimMontage(anim);
	}
	else if (RightScaleValue > 0.1)
	{
		if (DodgeAnims.Num() >= 2)
		{
			CharState = State_Dodge;
			UAnimMontage* anim = DodgeAnims[1];
			PlayAnimMontage(anim);
		}
	}

	if (ForwardScaleValue < -0.1 || fabs(RightScaleValue) < 0.1)
	{
		if (DodgeAnims.Num() >= 3)
		{
			CharState = State_Dodge;
			UAnimMontage* anim = DodgeAnims[2];
			PlayAnimMontage(anim);
		}
	}
}
