// Copyright Quarantine Games 2020


#include "TurretAI.h"
#include "Components/StaticMeshComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "..\Player\BasePlayer.h"
#include "AIController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "..\Interfaces/RaycastNotifyable.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/DamageType.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundBase.h"
#include "Components/ArrowComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "..\Components/HealthComponent.h"
#include "Projectiles\TurretLaserProjectile.h"
#include "AIEnemy.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Components/PrimitiveComponent.h"

// Sets default values
ATurretAI::ATurretAI()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	pCapsuleCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleRoot"));
	pCapsuleCollider->SetIsReplicated(true);
	SetRootComponent(pCapsuleCollider);
	//RootComponent = pCapsuleCollider;

	pMeshTop = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurretTop"));
	pMeshTop->SetupAttachment(RootComponent);
	pMeshTop->BodyInstance.SetCollisionProfileName("BlockAll");
	pMeshTop->SetIsReplicated(true);


	pMeshBottom = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurretBase"));
	pMeshBottom->SetupAttachment(RootComponent);
	pMeshBottom->SetIsReplicated(true);

	//pHealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComp"));
	//pHealthComp->fDefaultShieldAmount = 0.f;
	//pHealthComp->fShieldAmount = 0.f;
	pHealthComp->DefaultHealth = 120.0f;
	InitialHealth = pHealthComp->DefaultHealth;



	pArrow2 = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow2"));
	pArrow2->SetupAttachment(pCapsuleCollider);

	pLaserParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleComp"));


	//bAlwaysRelevant = true;
	//SetReplicates(true);
	//SetReplicateMovement(true);

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	PawnSensingComp->OnSeePawn.AddDynamic(this, &ATurretAI::OnPawnSeen);

	fPitchValue = 0.f;
	fYawValue = .32f;
	fRollValue = 0.f;

	bRotateDirection = false;
	InitialRotation = GetActorRotation();

	TracerTargetName = "Target";

	fTimeBetweenShots = 0.08;
	fTimeBetweenShotsProjectile = 1.f;

	fBulletSpread = 5;
	fWeaponRange = 20000.f;
	fItemLoudness = .32f;
	fBaseDamage = 3.2f;

	fTurretSpeed = 0.4f;

	fTurnAngle = 50.f;

	GetCapsuleComponent()->SetupAttachment(RootComponent);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	pCapsuleCollider->SetEnableGravity(false);
	GetCapsuleComponent()->SetSimulatePhysics(false);
	//GetCapsuleComponent()->SetEnableGravity(false);

	//INcrease stats over game - check this out
	fHealthIncrease = 1.2f;
}


// Called when the game starts or when spawned
void ATurretAI::BeginPlay()
{
	Super::BeginPlay();

	InitialRotation = GetActorRotation();
	InitialForwardVector = GetActorLocation() + GetActorForwardVector() * 320.f;

	if (GetLocalRole() == ROLE_Authority)
	{
		//pHealthComp->OnHealthChanged.AddDynamic(this, &ATurretAI::OnHealthChanged);
		//GetWorldTimerManager().SetTimer(TimerHandle_SwitchDirection, this, &ATurretAI::SwitchDirection, 4.f, true);
	}
	FTimerHandle TimerHandle_StartTrigger;
	GetWorldTimerManager().SetTimer(TimerHandle_StartTrigger, this, &ATurretAI::StartTrigger, 6.f, false);

	SetupLaser();
}

void ATurretAI::StartTrigger()
{
	//starts rotation so both clients and servers match
	if (GetLocalRole() == ROLE_Authority)
	{
		bStartTrigger = true;
		//GetWorldTimerManager().SetTimer(TimerHandle_SwitchDirection, this, &ATurretAI::SwitchDirection, 4.f, true);
	}

}

void ATurretAI::OnHealthChangedInternal(UHealthComponent* InHealthComp, float Health, float HealthDelta, const UDamageType* DamageType2, AController* InstigatedBy, AActor* DamageCauser)
{
	AAIController* controller = Cast<AAIController>(GetController());

	playerSeen = Cast<ABasePlayer>(DamageCauser);
	if (playerSeen)
	{
		playerSeen = Cast<ABasePlayer>(DamageCauser);
		bPlayerSeen = true;
		GetWorldTimerManager().ClearTimer(TimerHandle_ResetRotation);
	}

	if (Health <= 0)
	{
		MulticastDeathExtras();
		Destroy();
	}
}

void ATurretAI::OnPawnSeen(APawn* SeenPawn)
{
	AAIController* controller = Cast<AAIController>(GetController());

	if (Cast<ABasePlayer>(SeenPawn) && Cast<ABasePlayer>(SeenPawn)->GetIsPlayerDowned() == false)
	{
		playerSeen = Cast<ABasePlayer>(SeenPawn);
		bPlayerSeen = true;
		GetWorldTimerManager().ClearTimer(TimerHandle_ResetRotation);
	}
}

void ATurretAI::SwitchDirection()
{

	if (bRotateDirection)
	{
		bRotateDirection = false;
	}
	else if (!bRotateDirection && bResetTurret == false)
	{
		bRotateDirection = true;
	}
}

void ATurretAI::FirePrepare()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (playerSeen)
		{
			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(this);
			QueryParams.bTraceComplex = true;

			FVector RaycastEnd2 = playerSeen->GetActorLocation();
			//FVector RaycastBegin2 = GetActorLocation();
			FVector RaycastBegin2 = pMeshTop->GetSocketLocation("Muzzle");

			FHitResult Hit;
			if (GetWorld()->LineTraceSingleByChannel(Hit, RaycastBegin2, RaycastEnd2, ECC_Visibility, QueryParams))
			{
				ABasePlayer* basePlayer = Cast<ABasePlayer>(Hit.GetActor());
				if (basePlayer && basePlayer->GetIsPlayerDowned() == false)
				{
					if (!bLazerTurret)
					{
						FireCommence();
					}
					else
					{
						FireCommenceProjectile();
					}
				}
			}
		}
	}
}


void ATurretAI::FireCommence()
{
	if (fFireTimer <= 0 && bPatrolMode == false)
	{
		fFireTimer = fTimeBetweenShots;

		FVector nuthin;
		FRotator EyeRotation;
		GetActorEyesViewPoint(nuthin, EyeRotation);
		FVector EyeLocation = pMeshTop->GetSocketLocation("Muzzle");

		FVector ShotDirection = EyeRotation.Vector();

		// Bullet Spread
		float HalfRad = FMath::DegreesToRadians(fBulletSpread);
		ShotDirection = FMath::VRandCone(ShotDirection, HalfRad, HalfRad);

		FVector TraceEnd = EyeLocation + (ShotDirection * 10000);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;

		FVector TracerEndPoint = TraceEnd;


		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, ECC_Visibility, QueryParams))
		{
			AActor* HitActor = Hit.GetActor();
			ABasePlayer* basePlayer = Cast<ABasePlayer>(HitActor);
			if (basePlayer)
			{
				//basePlayer->MakeNoise(0.75f, nullptr, GetActorLocation()); //gets noise from weapon
				UGameplayStatics::ApplyPointDamage(basePlayer, fBaseDamage, ShotDirection, Hit, GetInstigatorController(), this, DamageType);

			}

			IRaycastNotifyable* rayNotifyable = Cast<IRaycastNotifyable>(Hit.GetActor()); //Nicks code
			if (rayNotifyable)
			{
				rayNotifyable->NotifyHit(GetOwner(), &Hit);
			}

			TracerEndPoint = Hit.ImpactPoint;

			MulticastFireExtras(TracerEndPoint);
		}
	}
}

void ATurretAI::FireCommenceProjectile()
{
	if (fFireTimer <= 0 && bPatrolMode == false)
	{
		fFireTimer = fTimeBetweenShotsProjectile;

		FRotator SpawnRotation = pMeshTop->GetSocketRotation("Muzzle");
		FVector SpawnLocation = pMeshTop->GetSocketLocation("Muzzle");
		//FRotator SpawnRotation2 = humanOwner->GetControlRotation() + FRotator(1.f, 4.f, 0.f);

		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.Owner = this;
		ActorSpawnParams.Instigator = this;

		// spawn the projectile at the muzzle
		ATurretLaserProjectile* projectile = GetWorld()->SpawnActor<ATurretLaserProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		projectile->fBaseDamage = fBaseDamage * 5;

		//Empty vector (no stream effect)
		MulticastFireExtras(FVector(0.f, 0.f, 0.f));
	}
}


// Called every frame
void ATurretAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if (playerSeen && playerSeen->GetHealthComponent()->Health == 0)
	//{
	//	//playerSeen = nullptr;
	//}

	if (GetActorRotation().Yaw > fTurnAngle)
	{
		bRotateDirection = false;
	}
	if (GetActorRotation().Yaw < -fTurnAngle)
	{
		bRotateDirection = true;
	}

	if (bPlayerSeen == false && bStartTrigger) //if player has not been see, rotate
	{
		bPatrolMode = true;
		if (bRotateDirection)
		{
			FQuat QuatRotation = FQuat(FRotator(0.f, fYawValue, 0.f));
			AddActorLocalRotation(QuatRotation, false, 0, ETeleportType::None);
		}
		if (bRotateDirection == false)
		{
			FQuat QuatRotation = FQuat(FRotator(0.f, -fYawValue, 0.f));
			AddActorLocalRotation(QuatRotation, false, 0, ETeleportType::None);
		}
	}
	
	

	AAIController* controller = Cast<AAIController>(GetController());

	if (bPlayerSeen == true && playerSeen != nullptr) //If player is seen,
	{
		bPatrolMode = false;
		//FVector destination = playerSeen->GetActorLocation(); //works
		//FVector viewDir = destination - GetActorLocation();
		//FRotator PlayerRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), destination);
		//FRotator Rot = FRotationMatrix::MakeFromX(viewDir).Rotator();
		//SetActorRotation(Rot);

		FVector MyLoc = GetActorLocation();
		FVector TargetLoc = playerSeen->GetActorLocation();
		FVector Dir = (TargetLoc - MyLoc);
		Dir.Normalize();
		SetActorRotation(FMath::Lerp(GetActorRotation(), Dir.Rotation(), fTurretSpeed));

		FVector MuzzleLocation = pMeshTop->GetSocketLocation("Muzzle");

		if (controller && !controller->LineOfSightTo(playerSeen, MuzzleLocation)) //If player runs out of line
		{
			//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, "Out Of Sight!");

			playerSeen = nullptr;
			GetWorldTimerManager().SetTimer(TimerHandle_ResetRotation, this, &ATurretAI::ResetTurret, 2.0f, false);
			//GetWorldTimerManager().ClearTimer(TimerHandle_SwitchDirection);
		}

	}	

	fFireTimer -= DeltaTime;
	FirePrepare();

	if (bResetTurret)
	{
		FVector MyLoc = GetActorLocation();
		FVector TargetLoc = InitialForwardVector;
		FVector Dir = (TargetLoc - MyLoc);
		Dir.Normalize();
		SetActorRotation(FMath::Lerp(GetActorRotation(), Dir.Rotation(), fTurretSpeed));
	}

	pHealthComp->DefaultHealth *= fHealthIncrease; //Health Difficulty increase

	//PROBLEM
	if (playerSeen && playerSeen->GetIsPlayerDowned()) //if player down, reset turret
	{
		playerSeen = nullptr;
		FTimerHandle TimerHandle_ResetTurretLocal;
		//GetWorldTimerManager().SetTimer(TimerHandle_ResetRotation, this, &ATurretAI::ResetTurret, 1.0f, false);
		GetWorldTimerManager().SetTimer(TimerHandle_ResetTurretLocal, this, &ATurretAI::ResetTurret, 1.0f, false);
		//GetWorldTimerManager().ClearTimer(TimerHandle_SwitchDirection);
	}

	TickLaser(); //DUNNO
}

void ATurretAI::ResetTurret()
{
	//SetActorRotation(InitialRotation);
	bPlayerSeen = false;
	bRotateDirection = false;
	
	//resets the rotation in tick over seconds
	bResetTurret = true;
	FTimerHandle TimerHandle_ResetPosition;
	GetWorldTimerManager().SetTimer(TimerHandle_ResetPosition, this, &ATurretAI::ResetTurretPositionTimer, 1.2f, false);

}

void ATurretAI::ResetTurretPositionTimer()
{
	bResetTurret = false;
	bRotateDirection = false;
	//GetWorldTimerManager().SetTimer(TimerHandle_SwitchDirection, this, &ATurretAI::SwitchDirection, 4.f, true);
	
}

void ATurretAI::MulticastFireExtras_Implementation(FVector TracerEndPoint)
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), pFireSound, GetActorLocation());

	if (bLazerTurret)
	{
		return;
	}
	if (pTracerEffect)
	{
		FVector MuzzleLocation = pMeshTop->GetSocketLocation("Muzzle");

		UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), pTracerEffect, MuzzleLocation);
		if (TracerComp)
		{
			TracerComp->SetVectorParameter(TracerTargetName, TracerEndPoint);
		}
	}
}

bool ATurretAI::MulticastFireExtras_Validate(FVector TracerEndPoint)
{
	return true;
}

void ATurretAI::MulticastDeathExtras_Implementation()
{
	/*UGameplayStatics::PlaySoundAtLocation(GetWorld(), pDeathSound, GetActorLocation());

	if (pDeathExplosionEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), pDeathExplosionEffect, GetActorLocation());
	}*/
}

bool ATurretAI::MulticastDeathExtras_Validate()
{
	return true;
}

// Called to bind functionality to input
void ATurretAI::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


void ATurretAI::SetupLaser()
{
	//UParticleSystemComponent* SpawnEmitterAttached;
	////SpawnEmitterAttached = UGameplayStatics::SpawnEmitterAttached(CastChecked<UParticleSystem>(CastChecked<UDynamicClass>(ATurretLaserGun_BP_C__pf1243190021::StaticClass())->UsedAssets[10], ECastCheckedType::NullAllowed), pMeshTop, FName(TEXT("None")), FVector(0.000000, 0.000000, 0.000000), FRotator(0.000000, 0.000000, 0.000000), FVector(1.000000, 1.000000, 1.000000), EAttachLocation::KeepRelativeOffset, true, EPSCPoolMethod::None, true);
	//SpawnEmitterAttached = UGameplayStatics::SpawnEmitterAttached(CastChecked<UParticleSystem>(CastChecked<UDynamicClass>(ATurretAI::StaticClass())->UsedAssets[10], ECastCheckedType::NullAllowed), pMeshTop, FName(TEXT("None")), FVector(0.000000, 0.000000, 0.000000), FRotator(0.000000, 0.000000, 0.000000), FVector(1.000000, 1.000000, 1.000000), EAttachLocation::KeepRelativeOffset, true, EPSCPoolMethod::None, true);
	//pLaserParticle = SpawnEmitterAttached;
}


void ATurretAI::MulticastLaser_Implementation()
{
	if (pArrow2 && pLaserParticle)
	{
		FVector GetSocketLocation = pMeshTop->GetSocketLocation("Muzzle");

		pLaserParticle->SetBeamSourcePoint(0, GetSocketLocation, 0);

		FVector GetForwardVector2 = pArrow2->GetForwardVector();
		FVector VectorFloat2 = UKismetMathLibrary::Multiply_VectorFloat(GetForwardVector2, 2000.f);
		FVector VectorVector = UKismetMathLibrary::Add_VectorVector(GetSocketLocation, VectorFloat2);
		bool LineTraceSingleReturn = UKismetSystemLibrary::LineTraceSingle(this, GetSocketLocation, VectorVector, ETraceTypeQuery::TraceTypeQuery1, false, pobjectVariable, EDrawDebugTrace::None, /*out*/ LineTraceSingle_OutHit, true, FLinearColor(1.000000, 0.000000, 0.000000, 1.000000), FLinearColor(0.000000, 1.000000, 0.000000, 1.000000), 5.000000);
		
		UGameplayStatics::BreakHitResult(LineTraceSingle_OutHit, /*out*/ bBlockingHit, /*out*/ bInitialOverlap, /*out*/ fTime, /*out*/ fDistance, /*out*/ BreakHitResult_Location, /*out*/ BreakHitResult_ImpactPoint, /*out*/ BreakHitResult_Normal, /*out*/ BreakHitResult_ImpactNormal, /*out*/ pPhysMat, /*out*/ pHitActor, /*out*/ pHitComponent, /*out*/ HitBoneName, /*out*/ HitItem, /*out*/ FaceIndex, /*out*/ TraceStart2, /*out*/ TraceEnd2);
		//bpfv__CallFunc_SelectVector_ReturnValue__pf = UKismetMathLibrary::SelectVector(b0l__CallFunc_BreakHitResult_Location__pf, b0l__CallFunc_BreakHitResult_TraceEnd__pf, b0l__CallFunc_BreakHitResult_bBlockingHit__pf);
		FVector SelectVector = UKismetMathLibrary::SelectVector(BreakHitResult_Location, TraceEnd2, bBlockingHit);
		
		FVector ForwardVector = pArrow2->GetForwardVector() * 3200.f;

		pLaserParticle->SetBeamTargetPoint(0, SelectVector, 0);
		//pLaserParticle->SetBeamTargetPoint(0, ForwardVector, 0);
	}

}

void ATurretAI::TickLaser()
{
	MulticastLaser();

	if (pArrow2 && pLaserParticle)
	{
		FVector GetSocketLocation = pMeshTop->GetSocketLocation("Muzzle");

		pLaserParticle->SetBeamSourcePoint(0, GetSocketLocation, 0);

		FVector GetForwardVector2 = pArrow2->GetForwardVector();
		FVector VectorFloat2 = UKismetMathLibrary::Multiply_VectorFloat(GetForwardVector2, 2000.f);
		FVector VectorVector = UKismetMathLibrary::Add_VectorVector(GetSocketLocation, VectorFloat2);
		bool LineTraceSingleReturn = UKismetSystemLibrary::LineTraceSingle(this, GetSocketLocation, VectorVector, ETraceTypeQuery::TraceTypeQuery1, false, pobjectVariable, EDrawDebugTrace::None, /*out*/ LineTraceSingle_OutHit, true, FLinearColor(1.000000, 0.000000, 0.000000, 1.000000), FLinearColor(0.000000, 1.000000, 0.000000, 1.000000), 5.000000);

		UGameplayStatics::BreakHitResult(LineTraceSingle_OutHit, /*out*/ bBlockingHit, /*out*/ bInitialOverlap, /*out*/ fTime, /*out*/ fDistance, /*out*/ BreakHitResult_Location, /*out*/ BreakHitResult_ImpactPoint, /*out*/ BreakHitResult_Normal, /*out*/ BreakHitResult_ImpactNormal, /*out*/ pPhysMat, /*out*/ pHitActor, /*out*/ pHitComponent, /*out*/ HitBoneName, /*out*/ HitItem, /*out*/ FaceIndex, /*out*/ TraceStart2, /*out*/ TraceEnd2);
		//bpfv__CallFunc_SelectVector_ReturnValue__pf = UKismetMathLibrary::SelectVector(b0l__CallFunc_BreakHitResult_Location__pf, b0l__CallFunc_BreakHitResult_TraceEnd__pf, b0l__CallFunc_BreakHitResult_bBlockingHit__pf);
		FVector SelectVector = UKismetMathLibrary::SelectVector(BreakHitResult_Location, TraceEnd2, bBlockingHit);

		FVector ForwardVector = pArrow2->GetForwardVector() * 3200.f;

		pLaserParticle->SetBeamTargetPoint(0, SelectVector, 0);
		//pLaserParticle->SetBeamTargetPoint(0, ForwardVector, 0);
	}
}

void ATurretAI::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	/*DOREPLIFETIME(ATurretAI, pCapsuleCollider);
	DOREPLIFETIME(ATurretAI, playerSeen);
	DOREPLIFETIME(ATurretAI, bPlayerSeen);
	DOREPLIFETIME(ATurretAI, InitialForwardVector);
	DOREPLIFETIME(ATurretAI, InitialRotation);
	DOREPLIFETIME(ATurretAI, bResetTurret);
	DOREPLIFETIME(ATurretAI, fTurretSpeed);
	DOREPLIFETIME(ATurretAI, fFireTimer);
	DOREPLIFETIME(ATurretAI, bStartTrigger);
	DOREPLIFETIME(ATurretAI, bPlayerDead);*/

	//DOREPLIFETIME(ATurretAI, fYawValue);
	//DOREPLIFETIME(ATurretAI, pMeshTop);
	//DOREPLIFETIME(ATurretAI, pMeshBottom);
	//DOREPLIFETIME(ATurretAI, bRotateDirection);
	DOREPLIFETIME(ATurretAI, pLaserParticle);
	DOREPLIFETIME(ATurretAI, pArrow2);
	//DOREPLIFETIME(ATurretAI, InitialForwardVector);
	//DOREPLIFETIME(ATurretAI, InitialRotation);
	//DOREPLIFETIME(ATurretAI, fFireTimer);
}
