// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ZombieBrains.generated.h"

/**
 * 
 */
UCLASS()
class TANKSVSZOMBIES_API AZombieBrains : public AAIController
{
	GENERATED_BODY()

public:

	virtual void Tick(float DeltaSeconds) override;

	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnUnPossess() override;

protected:
	AZombie* PawnAsZombie; 
};
