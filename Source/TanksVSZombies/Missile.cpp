// Fill out your copyright notice in the Description page of Project Settings.


#include "Missile.h"
#include "DamageInterface.h"

// Sets default values
AMissile::AMissile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Speed = 200.f;
	Radius = 20.f;
	DirectDamage = 5;
}

// Called when the game starts or when spawned
void AMissile::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(ExplodeTimerHandle, this, &AMissile::Explode, 1.0f);
}

// Called every frame

void AMissile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	const FVector OurLocation = GetActorLocation();
	FVector DesiredEndLocation = OurLocation + ((DeltaSeconds * Speed) * GetTransform().GetUnitAxis(EAxis::X));
	FHitResult HitResult;
	FCollisionShape CollisionShape;
	// Set the Radius to 0 temporarily if you want a mortar that flies over stuff without colliding.
	if (Radius > 0.f)
	{
		if (UWorld* World = GetWorld())
		{
			CollisionShape.SetCapsule(Radius, 200.f);
			if (World->SweepSingleByProfile(HitResult, OurLocation, DesiredEndLocation
				, FQuat::Identity, MovementCollisionProfile, CollisionShape))
			{
				SetActorLocation(HitResult.Location);
				if (IDamageInterface* DamageActor = Cast<IDamageInterface>(HitResult.GetActor()))
				{
					// could make these arguments into class parameters so child types could customize.
					// for scalability, could take a struct instead of individual params.
					DamageActor->ReceiveDamage(DirectDamage, EDamageType::HitWithMissle);
				}
				Explode();
			}
			else
			{
				SetActorLocation(DesiredEndLocation);
			}
		}
	}
}

void AMissile::Explode()
{
	GetWorldTimerManager().ClearTimer(ExplodeTimerHandle);
	SetActorEnableCollision(false);
	OnExplode();
}

void AMissile::SomeEvent_Implementation()
{
}

void AMissile::OnExplode_Implementation()
{
	Destroy();
}
