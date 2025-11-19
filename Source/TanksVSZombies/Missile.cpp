// Fill out your copyright notice in the Description page of Project Settings.


#include "Missile.h"

// Sets default values
AMissile::AMissile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Speed = 200.f;
	Radius = 20.f;
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

	if (UWorld* World = GetWorld())
	{
		FHitResult OutHit;
		FCollisionShape CollisionShape;
		CollisionShape.SetCapsule(Radius, 200.f);
		if (World->SweepSingleByProfile(OutHit, OurLocation, DesiredEndLocation
			, FQuat::Identity, MovementCollisionProfile, CollisionShape))
		{
			SetActorLocation(OutHit.Location);
			Explode();
		}
		else
		{
			SetActorLocation(DesiredEndLocation);
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
