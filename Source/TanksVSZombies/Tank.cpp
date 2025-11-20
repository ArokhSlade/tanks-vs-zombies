// Fill out your copyright notice in the Description page of Project Settings.


#include "Tank.h"
#include "PaperSpriteComponent.h"
#include "TankStatics.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/SpringArmComponent.h"

void FTankInput::Sanitize()
{
	MovementInput = RawMovementInput;//.ClampAxes(-1.0f, 1.0f);
	//MovementInput = MovementInput.GetSafeNormal();
	RawMovementInput.Set(0.0f, 0.0f);
}

void FTankInput::MoveX(float AxisValue)
{
	RawMovementInput.X += AxisValue;
}

void FTankInput::MoveY(float AxisValue)
{
	RawMovementInput.Y += AxisValue;
}

void FTankInput::ShootPrimary(bool bPressed)
{
	bShootPrimary = bPressed;
}

void FTankInput::ShootSecondary(bool bPressed)
{
	bShootPrimary = bPressed;
}

// Sets default values
ATank::ATank()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    if (!RootComponent)
	{
    	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));		
	}
	
	TankDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("TankDirection"));
	TankDirection->SetupAttachment(RootComponent);


	TankBody = CreateDefaultSubobject<UBoxComponent>(TEXT("TankBody"));
	TankBody->SetupAttachment(TankDirection);
	TankBody->SetBoxExtent(FVector(40.f, 40.f, 100.f));
	
	TankSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("TankSprite"));
	TankSprite->SetupAttachment(TankDirection);

	USpringArmComponent* SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->TargetArmLength = 500.0f;
    SpringArm->bEnableCameraLag = true;
    SpringArm->bEnableCameraRotationLag = false;
	SpringArm->bUsePawnControlRotation = false;
    SpringArm->CameraLagSpeed = 2.0f;
    SpringArm->bDoCollisionTest = false;
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetWorldRotation(FRotator(-90.0f, 0.0f, 0.0f));

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->bUsePawnControlRotation = false;
	CameraComponent->ProjectionMode = ECameraProjectionMode::Orthographic;
	CameraComponent->OrthoWidth = 1024.0f;
	CameraComponent->AspectRatio = 3.0f / 4.0f;
	CameraComponent->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	CameraComponent->SetWorldRotation(FRotator(-90.0f, -90.0f, 0.0f));

    ChildTurret = CreateDefaultSubobject<UChildActorComponent>(TEXT("ChildTurret"));
	ChildTurret->SetupAttachment(TankDirection);

	MoveSpeed = 100.f;
	MoveAccel = 200.f;
	YawSpeed = 180.f;

	Health = 100;

	CrushCollisionProfile = TEXT("Tank:Crush");
}

// Called when the game starts or when spawned
void ATank::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TankInput.Sanitize();

	// Respond to controls if we're not dead!
	if (GetHealthRemaining() >= 0)
	{
		FVector DesiredMovementDirection = FVector(TankInput.MovementInput.X, TankInput.MovementInput.Y, 0.f);
		if (!DesiredMovementDirection.IsNearlyZero())
		{
			// Rotate the tank! Note that we rotate the TankDirection component,
			// not the RootComponent.
			FRotator MovementAngle = DesiredMovementDirection.Rotation(); //requires 3D Vector
			float DeltaYaw = UTankStatics::FindDeltaAngleDegrees(TankDirection->GetComponentRotation().Yaw, MovementAngle.Yaw);
			bool bReverse = false;
			if (DeltaYaw != 0.0f)
			{
				float AdjustedDeltaYaw = DeltaYaw;

				if (AdjustedDeltaYaw < -90.f)
				{
					AdjustedDeltaYaw += 180.f;
					bReverse = true;
				}
				else if (AdjustedDeltaYaw > 90.f)
				{
					AdjustedDeltaYaw -= 180.f;
					bReverse = true;
				}

				//Turn toward the desired angle. Stop if we can get there in one frame.
				float MaxYawThisFrame = YawSpeed * DeltaTime;;
				if (MaxYawThisFrame >= FMath::Abs(AdjustedDeltaYaw))
				{
					if (bReverse)
					{
						FRotator FacingAngle = MovementAngle;
						FacingAngle.Yaw = MovementAngle.Yaw + 180.f; //the engine will figure it out automatically
						TankDirection->SetWorldRotation(FacingAngle); 
					}
					else
					{
						TankDirection->SetWorldRotation(MovementAngle);
					}
				}
				else
				{
					// Can't reach our desired angle this frame, rotate part way.
					TankDirection->AddLocalRotation(FRotator(0.f, FMath::Sign(AdjustedDeltaYaw) * MaxYawThisFrame, 0.f));
				}
			}

			// Move the tank
			{
				FVector MovementDirection = TankDirection->GetForwardVector() * (bReverse ? -1.f : 1.f);
				FVector StartPos = GetActorLocation();
				FVector Pos = GetActorLocation();
				Pos.X += MovementDirection.X * MoveSpeed * DeltaTime;
				Pos.Y += MovementDirection.Y * MoveSpeed * DeltaTime;

				if (UWorld* World = GetWorld())
				{
					TArray<FHitResult> HitResults;
					FVector BoxSize = TankBody->GetScaledBoxExtent();
					FCollisionShape CollisionShape;
					CollisionShape.SetBox(FVector3f{BoxSize});
					World->SweepMultiByProfile(HitResults, StartPos, Pos, TankBody->GetComponentRotation().Quaternion(),
						CrushCollisionProfile, CollisionShape);
					for (const FHitResult& HitResult : HitResults)
					{
						if (IDamageInterface* DamageTarget = Cast<IDamageInterface>(HitResult.GetActor()))
						{
							// Getting crushed by a tank is pretty final. Damage is always enough to smoosh the raspberry jelly out of a zombie.
							int32 TargetHealth = DamageTarget->GetHealthRemaining();
							if (TargetHealth >= 0)
							{
								DamageTarget->ReceiveDamage(TargetHealth, EDamageType::Crushed);
							}
							
						}
					}
				}
				SetActorLocation(Pos);
			}
		}
	}
}

// Called to bind functionality to input
void ATank::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	InputComponent->BindAxis(TEXT("MoveX"), this, &ATank::MoveX);
	InputComponent->BindAxis(TEXT("MoveY"), this, &ATank::MoveY);
	InputComponent->BindAction(TEXT("ShootPrimary"), IE_Pressed, this, &ATank::ShootPrimaryPressed);
	InputComponent->BindAction(TEXT("ShootPrimary"), IE_Released, this, &ATank::ShootPrimaryReleased);
	InputComponent->BindAction(TEXT("ShootSecondary"), IE_Pressed, this, &ATank::ShootSecondaryPressed);
	InputComponent->BindAction(TEXT("ShootSecondary"), IE_Released, this, &ATank::ShootSecondaryReleased);
}

void ATank::DamageHealth(int damage)
{
	//TODO
}

void ATank::MoveX(float AxisValue)
{
	TankInput.MoveX(AxisValue);
}

void ATank::MoveY(float AxisValue)
{
	TankInput.MoveY(AxisValue);
}

void ATank::ShootPrimaryPressed()
{
	TankInput.ShootPrimary(true);
}

void ATank::ShootPrimaryReleased()
{
	TankInput.ShootPrimary(false);
}

void ATank::ShootSecondaryPressed()
{
	TankInput.ShootSecondary(true);
}

void ATank::ShootSecondaryReleased()
{
	TankInput.ShootSecondary(false);
}

