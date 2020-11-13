// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

UCLASS()
class RPG_PROJECT2_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//[===Smooth Movement===]
	UFUNCTION(BlueprintCallable)
	void MoveForward(float axis_value, float start_acc, float stop_acc);

	UFUNCTION(BlueprintCallable)
	void MoveRight(float axis_value, float start_acc, float stop_acc);
	//[===Smooth Movement===]

	//[===Look At Location===]
	UFUNCTION(BlueprintCallable)
	void LookToLocation(FVector loc);

	UFUNCTION(BlueprintCallable)
	void LookToActor(AActor* actor);

	UFUNCTION(BlueprintCallable)
	void DisableLookAt();

	UFUNCTION(BlueprintCallable)
	void SetRotationSpeed(float rot_speed);
	//[===Look At Location===]

	//[===Move To Location===]
	UFUNCTION(BlueprintCallable)
	void MoveToLocation(FVector loc, float stop_dist, float start_acc, float stop_acc);

	UFUNCTION(BlueprintCallable)
	void MoveToActor(AActor *actor, float stop_dist, float start_acc, float stop_acc);

	UFUNCTION(BlueprintCallable)
	void DisableMoveTo();

	UFUNCTION(BlueprintCallable)
	bool IsMoveToEnabled();
	//[===Move To Location===]

	//[===Action Handling===]
	UFUNCTION(BlueprintCallable)
	void HandleActionFromChar(AActor *actor, FString action, 
		bool check_visibility, bool check_audibility);

	UFUNCTION(BlueprintCallable)
	void AddVisibleTarget(AActor* actor);
	UFUNCTION(BlueprintCallable)
	void RemoveVisibleTarget(AActor* actor);
	UFUNCTION(BlueprintCallable)
	ABaseCharacter* GetHandlingTarget();
	//[===Action Handling===]

	UFUNCTION(BlueprintCallable)
	void SetMoveSpeed(float value);

	UPROPERTY(EditAnywhere)
	float CombatRange = 500.0f;
	UPROPERTY(EditAnywhere)
	float AttackRange = 200.0f;
	

private:
	void  UpdateMoveTo(float deltaTime);
	void  UpdateLookAt(float deltaTime);

	float ForwardScaleValue = 0;
	float RightScaleValue = 0;

	float StopDistance = 100.0f;
	float StartAcceleration = 0.0f;
	float StopAcceleration = 0.0f;
	float RotationSpeed = 2.0f;

	FVector LookTarget = FVector(0, 0, 0);
	FVector MoveTarget = FVector(0, 0, 0);
	AActor* ActorMoveTarget = nullptr;
	AActor* ActorLookTarget = nullptr;

	bool  IsMoveTo = false;
	bool  IsLookAt = false;

	TArray<ABaseCharacter*> VisibleTargets;
	ABaseCharacter* HandlingTarget = nullptr;
};
