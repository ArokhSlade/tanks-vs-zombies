// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Turret.generated.h"

class ATank;
class AMissile;
class UArrowComponent;

UCLASS()
class TANKSVSZOMBIES_API ATurret : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATurret();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	UArrowComponent* GetTurretDirection() { return TurretDirection; }

	/** The name of the socket at the muzzle - used for spawning missiles. */
	static const FName MuzzleSocketName;
	
protected:
	// turn rate in degrees/second for the turret.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret")
	float YawSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turret")
	ATank* Tank;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret")
	//TArray<TSubclassOf<AMissile>> Projectiles;
	TArray<TSubclassOf<AActor>> Projectiles;

private:
	// Helpful debug tool - which way is the turret facing?
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turret", meta = (AllowPrivateAccess = "true"))
	UArrowComponent* TurretDirection;

	// Sprite for the turret
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turret", meta = (AllowPrivateAccess = "true"))
	class UPaperSpriteComponent* TurretSprite;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret", meta = (AllowPrivateAccess = "true"))
	float ShootPrimaryCooldown;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Turret", meta = (AllowPrivateAccess = "true"))
	float ShootPrimaryReadyTime;
};
