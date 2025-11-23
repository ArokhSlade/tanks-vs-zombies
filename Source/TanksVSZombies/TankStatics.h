// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TankStatics.generated.h"

class UPaperFlipbookComponent;
class UPaperFlipbook;
/**
 * 
 */
UCLASS()
class TANKSVSZOMBIES_API UTankStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/** Calculate angle difference in the range [-180, 180]. */
	UFUNCTION(BlueprintCallable, Category = "Math")
	static float FindDeltaAngleDegrees(float A1, float A2);

	/** Finds the angle between two 2D points. */
	UFUNCTION(BlueprintPure, Category = "Math|Rotator", meta = (Keywords = "rotation rotate"))
	static bool FindLookAtAngle2D(const FVector2D& Start, const FVector2D& Target, float& Angle);

	UFUNCTION(BlueprintCallable, Category = "Placement", meta = (Keywords = "spawning"))
	static void PutInZPlane(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "Animation")
	static void PlayFlipBook(UPaperFlipbookComponent* Component, UPaperFlipbook* NewFlipbook, bool bLooping = false, float Playrate = 1.f);
};
