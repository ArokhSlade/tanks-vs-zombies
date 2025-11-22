// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageInterface.h"
#include "GameFramework/Pawn.h"
#include "Tank.generated.h"

// This struct covers all possible tank inpu tscheme.
// What the inputs do can vary by tank, but the same inputs will always exist.
USTRUCT(BlueprintType)
struct FTankInput
{
	GENERATED_BODY()

public:
	// Sanitized movement input, ready to be used by game logic.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank Input")
	FVector2D MovementInput;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank Input")
	uint32 bShootPrimary : 1;
	uint32 bShootSecondary : 1;

	void Sanitize();
	void MoveX(float AxisValue);
	void MoveY(float AxisValue);
	void ShootPrimary(bool bPressed);
	void ShootSecondary(bool bPressed);

private:
	// Private because it's internal, raw data. Game code should never see this.
	FVector2D RawMovementInput;
};

UCLASS()
class TANKSVSZOMBIES_API ATank : public APawn, public IDamageInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATank();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Tank")
	FORCEINLINE const FTankInput& GetCurrentInput() const {return TankInput;};

	//~ Begin IDamageInterface
	void ReceiveDamage(int32 IncomingDamage, EDamageType DamageType) override;	
	int32 GetHealthRemaining() override;
	//~ End IDamageInterface
	
	// when Tank die
	UFUNCTION(BlueprintNativeEvent, Category = "Tank")
	void TankDie(EDamageType DamageType);
	
private:
	void MoveX(float AxisValue);
	void MoveY(float AxisValue);
	void ShootPrimaryPressed();
	void ShootPrimaryReleased();
	void ShootSecondaryPressed();
	void ShootSecondaryReleased();
private:
	// Helpful debug tool - which way is the tank facing?
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank", meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* TankDirection;

	// Collision body for the tank.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* TankBody;
	
	// Sprite for the turret
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank", meta = (AllowPrivateAccess = "true"))
	class UPaperSpriteComponent* TankSprite;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank", meta = (AllowPrivateAccess = "true"))
	UChildActorComponent* ChildTurret;

	// Our in-game Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* CameraComponent;

protected:
	// Our input structure.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tank Input")
	FTankInput TankInput;

	// Maximum turn rate (degrees/second)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank", meta = (ClampMin = "0.0"))
	float YawSpeed;

	// Maximum movement rate (units/seconds) of the tank.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tank", meta = (ClampMin = "0.0"))
	float MoveSpeed;

	// Acceleration in units/second^2
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tank", meta = (ClampMin = "0.0"))
	float MoveAccel;

	// Hitpoints
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tank|Health", meta = (ClampMin = "0.0"))
	int Health;

	// Collision Profile for running over zombies
	UPROPERTY(EditAnywhere, BluepritnReadOnly, Category = "Tank|Combat", meta = (Keywords = "Collision, Carnage"))
	FName CrushCollisionProfile;
	
};
