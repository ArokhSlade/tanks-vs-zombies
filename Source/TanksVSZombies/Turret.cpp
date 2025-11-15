// Fill out your copyright notice in the Description page of Project Settings.


#include "Turret.h"
#include "PaperSpriteComponent.h"
#include "Tank.h"
#include "TankStatics.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATurret::ATurret()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = TurretDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("TurretDirection"));

	TurretSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("TurretSprite"));
	TurretSprite->SetupAttachment(TurretDirection);

	YawSpeed = 180.f;

	TurretDirection->SetHiddenInGame(false);
	TurretDirection->bIsEditorOnly = false;
	TurretDirection->SetVisibility(true);
}

// Called when the game starts or when spawned
void ATurret::BeginPlay()
{
	Super::BeginPlay();

	// Be sure to aim after the tanks has turned and moved so that the turret doesn't lag one frame behind.
	// Not needed to write because this ATurret being a ChildComponentActor, this is automatic
	ensure(IsChildActor());
	AddTickPrerequisiteActor(GetParentActor());
	Tank = Cast<ATank>(GetParentActor());
	ensure(Tank);
}

// Called every frame
void ATurret::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	ensure(TurretDirection);
	ensure(Tank);
	if (Tank)
	{
		if (APlayerController* PC = Cast<APlayerController>(Tank->GetController()))
		{
			FVector2D ScreenSpaceAimLocation{};
			if (PC->GetMousePosition(ScreenSpaceAimLocation.X, ScreenSpaceAimLocation.Y))
			{
				FVector2D ScreenSpaceTurretLocation = FVector2D::ZeroVector;
				UGameplayStatics::ProjectWorldToScreen(PC, TurretDirection->GetComponentLocation(), ScreenSpaceTurretLocation);

				auto ScreenToWorld2D = FMatrix2x2(0.f, 1.f, -1.f, 0.f);
				FVector2D AimLocation = ScreenToWorld2D.TransformPoint(ScreenSpaceAimLocation);
				FVector2D TurretLocation = ScreenToWorld2D.TransformPoint(ScreenSpaceTurretLocation);

				float DesiredYaw = 0.f;

				if (UTankStatics::FindLookAtAngle2D(TurretLocation, AimLocation, DesiredYaw))
				{
					FRotator CurrentRotation = TurretDirection->GetComponentRotation();
					const float DeltaYaw = UTankStatics::FindDeltaAngleDegrees(CurrentRotation.Yaw, DesiredYaw);
					if (const float MaxDeltaYawThisFrame = YawSpeed * DeltaSeconds; MaxDeltaYawThisFrame >= FMath::Abs(DeltaYaw))
					{
						CurrentRotation.Yaw += DeltaYaw;
					} else
					{
						CurrentRotation.Yaw += FMath::Sign(DeltaYaw) * MaxDeltaYawThisFrame;
					}
					TurretDirection->SetWorldRotation(CurrentRotation);
				}
			}
		}		
	}
}
