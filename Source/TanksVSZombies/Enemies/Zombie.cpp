// Fill out your copyright notice in the Description page of Project Settings.


#include "Zombie.h"
#include "Tank.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AZombie::AZombie()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	Health = 10;
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
	ZombieAI(DeltaTime);

	// Clear out leftover movement.
	ConsumeMovementInputVector();
	ConsumeRotationInput();
	ConsumeAttackInput();
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
	// Look for target. We might not do this every single frame, but for now it's OK.
	// TODO: Make this use a list of registered targets so we can handle multiplayer or add decoys.
	// TODO: sweep view cone for player.
	// -> check if player left of right edge, and right of left edge, and within sight distance 
	// if so set target to player
	// should happen aiming towards player.
		
	AActor* TargetCandidate = UGameplayStatics::GetPlayerPawn(this, 0);
	const float DistSqXY = FVector::DistSquaredXY(TargetCandidate->GetActorLocation(), GetActorLocation());
	SetTarget(nullptr);
	if (DistSqXY <= (SightDistance * SightDistance))
	{
		FVector DirectionToTarget = (TargetCandidate->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();
		if (FVector::DotProduct(DirectionToTarget, GetActorForwardVector()) >= FMath::Cos(FMath::DegreesToRadians(SightAngle)))
		{
			SetTarget(TargetCandidate);
		}
	}
	
	// The zombie always moves unless attacking. If moving, it moves between WalkSpeed and RunSpeed.
	const FVector DesiredMovement = GetAttackInput() ? FVector::ZeroVector
	: (FMath::GetMappedRangeValueClamped(FVector2D(0.f, 1.f),FVector2D(WalkSpeed, RunSpeed),GetPendingMovementInputVector().X)) * DeltaSeconds * GetActorForwardVector();
	const FVector OriginalLocation = GetActorLocation();
	const FVector DesiredLocation = OriginalLocation + DesiredMovement;
	const float MaxYawThisFrame = YawSpeed * DeltaSeconds;

	//TODO angle to target
	const FRotator DesiredRotation = GetActorRotation() + FRotator(0.f, FMath::Clamp(GetRotationInput(), -MaxYawThisFrame, MaxYawThisFrame), 0.f);

	SetActorLocationAndRotation(DesiredLocation, DesiredRotation.Quaternion(), true);
	const FVector DistanceWalked = GetActorLocation() - OriginalLocation;
	if (!DistanceWalked.IsNearlyZero())
	{
		ZombieWalk(DeltaSeconds, DistanceWalked);
	}

	if (AActor* Target = GetTarget())
	{
		const FVector OurLocation = GetActorLocation();
		const FVector DirectionToTarget = (Target->GetActorLocation() - OurLocation).GetSafeNormal2D();
		const float DotToTarget = FVector::DotProduct(DirectionToTarget,GetActorForwardVector());
		
		const float CurrentTime = GetWorld()->GetTimeSeconds();
		if (GetAttackInput() && (AttackAvailableTime <= CurrentTime))
		{
			AttackAvailableTime = CurrentTime + AttackCooldown;
			ZombieAttack(DeltaSeconds);
			if (DotToTarget >= FMath::Cos(FMath::DegreesToRadians(AttackAngle)))
			{
				//const float DistSqXY = FVector::DistSquaredXY(Target->GetActorLocation(), OurLocation);
				if (DistSqXY <= (AttackDistance * AttackDistance))
				{
					if (ATank* TankTarget = GetTargetAsTank())
					{
						TankTarget->ReceiveDamage(10, EDamageType::ZombieSlap);
						if (APlayerController* PC = Cast<APlayerController>(TankTarget->GetController()))
						{
							PC->ClientStartCameraShake(HitShake, 1.f);
						}
					}
					else
					{
						SetTarget(nullptr);
					}
				}
			}
		}
	}
}

bool AZombie::ZombieAIShouldAttack_Implementation()
{
	if (AActor* Target = GetTarget())
	{
		// Attack our target if we're in range (distance and angle). FOr now, we'll use our unmodified attack distance
		FVector OurLocation = GetActorLocation();
		FVector DirectionToTarget = (Target->GetActorLocation() - OurLocation).GetSafeNormal();
		auto forward_vector = GetActorForwardVector();
		float DotToTarget = FVector::DotProduct(DirectionToTarget,forward_vector);
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

float AZombie::GetSightAngle()
{
	return SightAngle;
}

void AZombie::ReceiveDamage(int32 IncomingDamage, EDamageType DamageType)
{
	if (IncomingDamage >= 0)
	{
		Health -= IncomingDamage;
		if (Health <= 0)
		{
			Destroy();
		}
	}
}

int32 AZombie::GetHealthRemaining()
{
	return Health;
}
