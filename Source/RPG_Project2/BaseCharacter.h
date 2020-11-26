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
	enum CharacterState {
		State_Idle, State_Attack, State_Block, State_Dodge
	};
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

	//[===COMBAT===]
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	float CombatRange = 500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackRange = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float ChanceReactIdle = 70.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float ChanceReactInAttack = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TArray<UAnimMontage*> AttackAnims;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TArray<UAnimMontage*> BlockAnims;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TArray<UAnimMontage*> DodgeAnims;

	UFUNCTION(BlueprintCallable)
	void BaseAttack();
	UFUNCTION(BlueprintCallable)
	void RandomAttack();
	UFUNCTION(BlueprintCallable)
	void BaseBlock();
	UFUNCTION(BlueprintCallable)
	void BaseDodge();

	void NextComboAttack();
	void EndAttack();
	int getComboCounter() { return ComboCounter; }
	int IsContinueCombo() { return ContinueCombo; }
	//[===COMBAT===]

	UFUNCTION(BlueprintCallable)
	void SetMoveSpeed(float value);

	
	

private:
	void  UpdateMoveTo(float deltaTime);
	void  UpdateLookAt(float deltaTime);

	float ForwardScaleValue = 0; //Current Forward Move Speed
	float RightScaleValue = 0;   //Current Right Move Speed

	float StopDistance = 100.0f;
	float StartAcceleration = 0.0f; //The acceleration with which the movement begins
	float StopAcceleration = 0.0f;  //The acceleration with which the movement ends
	float RotationSpeed = 2.0f;     //Speed of LookAt Rotation

	FVector LookTarget = FVector(0, 0, 0);	//The location the character is looking at
	FVector MoveTarget = FVector(0, 0, 0);	//Location to which the character is moving
	AActor* ActorLookTarget = nullptr;		//The actor the character is looking at
	AActor* ActorMoveTarget = nullptr;		//Actor to which the character is moving
	

	bool  IsMoveTo = false; //A flag indicating the character is looking at the target
	bool  IsLookAt = false; //A flag indicating that the character is moving towards the target

	TArray<ABaseCharacter*> VisibleTargets;		//Characters who are in sight
	ABaseCharacter* HandlingTarget = nullptr;	//The character on whose actions the reaction was made

	CharacterState	CharState = State_Idle;

	bool ContinueCombo = false;
	int	 ComboCounter = 0;
};
