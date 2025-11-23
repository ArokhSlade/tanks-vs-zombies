// Fill out your copyright notice in the Description page of Project Settings.


#include "TankStatics.h"
#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"

float UTankStatics::FindDeltaAngleDegrees(float A1, float A2)
{
	//Find the difference
	float Delta = A2 - A1;

	while (Delta > 180.0f)
	{
		Delta = Delta - 360.0f;
	}
	while (Delta < -180.0f)
	{
		Delta = Delta + 360.0f;
	}
	
	return Delta;
}

bool UTankStatics::FindLookAtAngle2D(const FVector2D& Start, const FVector2D& Target, float& Angle)
{
	FVector2D Normal = (Target - Start).GetSafeNormal();
	if (!Normal.IsNearlyZero())
	{
		Angle = FMath::RadiansToDegrees(FMath::Atan2(Normal.Y, Normal.X));
		return true;
	}
	return false;
}

void UTankStatics::PutInZPlane(AActor* Actor)
{
	if (Actor)
	{
		auto NewLocation = Actor->GetActorLocation();
		NewLocation.Z = 1.f;
		Actor->SetActorLocation(NewLocation);
	}
}

void UTankStatics::PlayFlipBook(UPaperFlipbookComponent* Component, UPaperFlipbook* NewFlipbook, bool bLooping,
	float Playrate)
{
	Component->SetFlipbook(NewFlipbook);
	Component->SetLooping(bLooping);
	Component->SetPlayRate(Playrate);
	Component->Play();
}
