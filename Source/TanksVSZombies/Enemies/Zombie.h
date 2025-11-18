// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Zombie.generated.h"

class ATank;

UCLASS()
class TANKSVSZOMBIES_API AZombie : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AZombie();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Set this zombie's target. The base version of this will handle updating TargetActor and TargetTank.
	UFUNCTION(BlueprintCallable, Category = "AI")
	void SetTarget(AActor* Target);
	
	// Return the target Actor. Returning nullptr indicates no target.
	UFUNCTION(BlueprintCallable, Category = "AI")
	AActor* GetTarget();

	// Return the target Actor as a Tank, if possible. Returning nullptr indicates no target, or that the target is not a Tank.
	UFUNCTION(BlueprintCallable, Category = "AI")
	ATank* GetTargetAsTank();

	// Zombies will call this on Tick.
	UFUNCTION(BlueprintNativeEvent, Category = "AI")
	void ZombieAI(float DeltaSeconds);
	
	UFUNCTION(BlueprintNativeEvent, Category = "AI")
	bool ZombieAIShouldAttack();

	// This can be used to animate walking.
	UFUNCTION(BlueprintImplementableEvent, Category = "AI")
	void ZombieWalk(float DeltaSeconds, FVector DistanceWalkedThisFrame);


	//~
	//~ New Zombie Input
	//~

	/** Add rotation equal to the yaw (in degrees) provided. */
	UFUNCTION(BlueprintCallable, Category = "Pawn|Input", meta = (Keywords = "AddInput"))
	void AddRotationInput(float DeltaYawDesired);

	/** Get rotation input. Returns pre-clear value. */
	UFUNCTION(BlueprintCallable, Category = "Pawn|Input", meta = (Keywords = "ConsumeInput"))
	float GetRotationInput();

	/** Get (and clear) rotation input. Returns pre-clear value. */
	UFUNCTION(BlueprintCallable, Category = "Pawn|Input", meta = (Keywords = "ConsumeInput"))
	float ConsumeRotationInput();

	/** Press the attack button. The pawn will know what to do with this. No arguments because it's a boolean ...*/
	UFUNCTION(BlueprintCallable, Category = "Pawn|Input", meta = (Keywords = "AddInput"))
	void AddAttackInput();

	/** Get the status of the attack button. */
	UFUNCTION(BlueprintCallable, Category = "Pawn|Input", meta = (Keywords = "ConsumeInput"))
	bool GetAttackInput();

	/** Get (and clear) the status of the attack button. */
	UFUNCTION(BlueprintCallable, Category = "Pawn|Input", meta = (Keywords = "ConsumeInput"))
	bool ConsumeAttackInput();

private:
	/* The actor we're targeting. Will be nullptr if there is no target. */
	UPROPERTY(VisibleInstanceOnly, Category="AI")
	AActor* TargetActor;

	/* The actor we're tareting, cast to a Tank and cached. Will be nullptr if no target or if the target actor is not a tank. */
	ATank* TargetTank;
	
protected:
	/** Camera effect, if any, to play when the player is hit by this Zombie. */
	UPROPERTY(EditAnywhere)
	TSubclassOf<UCameraShakeBase> HitShake;

	/** Current health value. Might be fun to have different values for different attack types, e.g. fire.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zombie", meta = (ClampMin = "0.0"))
	float Health;
	
	/** Sight distance (when no target is present)*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zombie", meta = (ClampMin = "0.0"))
	float SightDistance;
	
	/** Sight half-angle in degrees (when no target is present) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zombie", meta = (ClampMin = "0.0"))
	float SightAngle;
	//float SightAngleCos; // cached value to save conversions
	
	/** Max yaw speed per second in degrees*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zombie", meta = (ClampMin = "0.0"))
	float YawSpeed;
	
	/** Speed when wandering around, or when acing away from target.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zombie", meta = (ClampMin = "0.0"))
	float WalkSpeed;
	
	/** Speed when rushing toward target.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zombie", meta = (ClampMin = "0.0"))
	float RunSpeed;
	
	/** .*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zombie", meta = (ClampMin = "0.0"))
	float AttackDistance;
	
	/** Half-angle (in degrees) for our attack. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zombie", meta = (ClampMin = "0.0"))
	float AttackAngle;
	
	/** Minimum time between attacks.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zombie", meta = (ClampMin = "0.0"))
	float AttackCooldown;

	/** Game time, in seconds, when the Zombie will be allowed to attack again. */
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadWrite, Category = "Zombie")
	float AttackAvailableTime;

private:
	// Current rotation input;
	float YawInput;

	// Current attack button status.
	uint32 bAttackInput : 1;
};
