// Fill out your copyright notice in the Description page of Project Settings.


#include "Zombie.h"
#include "Tank.h"

// Sets default values
AZombie::AZombie()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	Health = 100.f;
	SightDistance = 500.f;
	SightAngle = 60.f;
	YawSpeed = 90.f;
	WalkSpeed = 25.f;
	RunSpeed = 45.f;
	AttackDistance = 100.f; // center-to-center
	AttackAngle = 30.f;
	AttackCooldown = 1.f;
}

// Called when the game starts or when spawned
void AZombie::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AZombie::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AZombie::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AZombie::SetTarget(AActor* Target)
{
	TargetActor = Target;
	TargetTank = Cast<ATank>(Target);
}

AActor* AZombie::GetTarget()
{
	return TargetActor;
}

ATank* AZombie::GetTargetAsTank()
{
	return TargetTank;
}

void AZombie::ZombieAI_Implementation(float DeltaSeconds)
{
	// The zombie always moves unless attacking. If moving, it moes between WalkSpeed and RunSpeed.
	//FVector DesiredMovement = GetAttackInput() ? FVector::ZeroVector : (FMath::GetMappedRangeValueClamped(FVector2D(0.f, 1.f),FVector2D(WalkSpeed, RunSpeed),GetPendingMovementInputVector().X)) * DeltaSeconds * GetActorForwardVector(); 
	const FVector DesiredMovement = GetAttackInput() ? FVector::ZeroVector :
	FMath::GetMappedRangeValueClamped(FVector2D(0.f, 1.f), FVector2D(WalkSpeed, RunSpeed),
		GetPendingMovementInputVector().Length()) * DeltaSeconds * GetActorForwardVector();
	const FVector OriginalLocation = GetActorLocation();
	const FVector DesiredLocation = OriginalLocation + DesiredMovement;
	const float MaxYawThisFrame = YawSpeed * DeltaSeconds;
	const FRotator DesiredRotation = GetActorRotation() + FRotator(0.f, FMath::Clamp(GetRotationInput(), -MaxYawThisFrame, MaxYawThisFrame), 0.f);

	SetActorLocationAndRotation(DesiredLocation, DesiredRotation.Quaternion(), true);
	const FVector DistanceWalked = GetActorLocation() - OriginalLocation;
	if (!DistanceWalked.IsNearlyZero())
	{
		ZombieWalk(DeltaSeconds, DistanceWalked);
	} 
}

bool AZombie::ZombieAIShouldAttack_Implementation()
{
	if (AActor* Target = GetTarget())
	{
		// Attack our target if we're in range (distance and angle). FOr now, we'll use our unmodified attack distance
		FVector OurLocation = GetActorLocation();
		FVector DirectionToTarget = (Target->GetActorLocation() - OurLocation).GetSafeNormal();
		float DotToTarget = FVector::DotProduct(DirectionToTarget,GetActorForwardVector());
		if (DotToTarget >= FMath::Cos(FMath::DegreesToRadians(AttackAngle)))
		{
			float DistSqXY = FVector::DistSquaredXY(OurLocation, Target->GetActorLocation());
			if (DistSqXY < AttackDistance * AttackDistance)
			{
				// Note that attacking cooldown isn't checked. We don't want this kind of zombie to move while it's waiting for an attack
				return true;
			}
		}
	}
	return false;
}

void AZombie::AddRotationInput(float DeltaYawDesired)
{
	YawInput += DeltaYawDesired;
}

float AZombie::GetRotationInput()
{
	return YawInput;
}

float AZombie::ConsumeRotationInput()
{
	float RetVal = YawInput;	
	YawInput = 0.f;
	return RetVal;
}

void AZombie::AddAttackInput()
{
	bAttackInput = true;
}

bool AZombie::GetAttackInput()
{
	return bAttackInput;
}

bool AZombie::ConsumeAttackInput()
{
	bool bRetVal = bAttackInput;
	bAttackInput = false;
	return bRetVal;
}
