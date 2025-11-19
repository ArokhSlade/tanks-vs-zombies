// Fill out your copyright notice in the Description page of Project Settings.

#include "ZombieBrains.h"
#include "Zombie.h"
//#include "TanksVSZombies/TanksVSZombies.h"


void AZombieBrains::Tick(float DeltaSeconds)
{	
	Super::Tick(DeltaSeconds);
	if (PawnAsZombie)
	{
		if (AActor* Target = PawnAsZombie->GetTarget())
		{
			// We do have a target. Shamble toward it and attempt violence!
			FVector DirectionToTarget = (Target->GetActorLocation() - PawnAsZombie->GetActorLocation()).GetSafeNormal2D();
			float DotToTarget = FVector::DotProduct(DirectionToTarget, PawnAsZombie->GetActorForwardVector());
			float SidewaysDotToTarget = FVector::DotProduct(DirectionToTarget, PawnAsZombie->GetActorRightVector());
			float DeltaYawDesired = FMath::Atan2(SidewaysDotToTarget, DotToTarget);

			if (PawnAsZombie->ZombieAIShouldAttack())
			{
					PawnAsZombie->AddAttackInput();
			}
			else
			{
				// Move faster when facing toward the target so that we turn more accurately/don't orbit.
				PawnAsZombie->AddMovementInput(FVector(1.f,0.f,0.f),
												FMath::GetMappedRangeValueClamped(FVector2D(-.707f, .707f), FVector2D(0.f, 1.f ), DotToTarget));
				// Attempt the entire turn in one frame. The Zombie itself will cap this, we're only expressing intent.
				PawnAsZombie->AddRotationInput(DeltaYawDesired);
			}
		}
	}
}

void AZombieBrains::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	PawnAsZombie = Cast<AZombie>(GetPawn());
}

void AZombieBrains::OnUnPossess()
{
	Super::OnUnPossess();
	PawnAsZombie = nullptr;
}
