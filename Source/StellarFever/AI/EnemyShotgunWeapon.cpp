// Copyright Quarantine Games 2020


#include "EnemyShotgunWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "BaseAICharacter.h"
#include "BaseAICharacter_AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Net/UnrealNetwork.h"
#include "RangeAI.h"
#include "BaseAICharacter.h"

AEnemyShotgunWeapon::AEnemyShotgunWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	fBaseDamage = 5.f;
	//InitialDamage = fBaseDamage;

	//fMaxAmmo = 128.f;
	//fCurrentAmmo = fMaxAmmo;
	fClipSizeAmount = 5;
	fCurrentAmmo = fClipSizeAmount;

	fFireTimer = 0;
	fBulletSpread = 32;
	fWeaponRange = 10000.f;

	fTimeBetweenShots = 3.2f;
	fReloadTimeDelay = 1.2f;

	fItemLoudness = 0.32f;
}

void AEnemyShotgunWeapon::ShotgunReload()
{
	MulticastReloadSound();
}

void AEnemyShotgunWeapon::BeginPlay()
{
	Super::BeginPlay();

	InitialDamage = fBaseDamage;

}

void AEnemyShotgunWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemyShotgunWeapon::FireSingleAI()
{
	//ForceNetUpdate();

	if (fFireTimer <= 0)
	{
		fFireTimer = fTimeBetweenShots;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_ReloadShotgun, this, &AEnemyShotgunWeapon::ShotgunReload, 1.0f, false);

		fCurrentAmmo--;

		if (fCurrentAmmo <= 0) //if ammo less then 0, reload
		{
			ABaseAICharacter* rangeAI = Cast<ABaseAICharacter>(GetOwner());
			if (rangeAI)
			{
				ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(rangeAI->GetController());
				if (controller)
				{
					fCurrentAmmo = fClipSizeAmount;
					controller->GetBlackboardComponent()->SetValueAsBool("AmmoRun", true);
				}
			}
		}

		UGameplayStatics::PlaySoundAtLocation(GetWorld(), pFireSound, GetActorLocation());

		AActor* MyOwner = GetOwner();
		if (MyOwner)
		{
			for (int i = 0; i < 6; i++)
			{
				FVector EyeLocation;
				FRotator EyeRotation;
				MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

				FVector ShotDirection = EyeRotation.Vector();

				// Bullet Spread
				float HalfRad = FMath::DegreesToRadians(fBulletSpread);
				ShotDirection = FMath::VRandCone(ShotDirection, HalfRad, HalfRad);

				FVector TraceEnd = EyeLocation + (ShotDirection * 10000);

				FCollisionQueryParams QueryParams;
				QueryParams.AddIgnoredActor(MyOwner);
				QueryParams.AddIgnoredActor(this);
				QueryParams.bTraceComplex = true;

				FVector TracerEndPoint = TraceEnd;

				FHitResult Hit;
				if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, ECC_Visibility, QueryParams))
				{
					AActor* HitActor = Hit.GetActor();

					UGameplayStatics::ApplyPointDamage(HitActor, fBaseDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), MyOwner, DamageType);

					TracerEndPoint = Hit.ImpactPoint;

					MulticastFireExtras(TracerEndPoint);

				}

				/*if (pTracerEffect)
				{
					FVector MuzzleLocation = pMeshComp->GetSocketLocation(MuzzleSocketName);

					UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), pTracerEffect, MuzzleLocation);
					if (TracerComp)
					{
						TracerComp->SetVectorParameter(TracerTargetName, TracerEndPoint);
					}
				}*/
			}
		}
	}

	//ForceNetUpdate();
}

void AEnemyShotgunWeapon::MulticastFireExtras_Implementation(FVector TracerEndPoint)
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), pFireSound, GetActorLocation());

	ABaseAICharacter* RangeOwner = Cast<ABaseAICharacter>(GetOwner());
	if (RangeOwner)
	{
		if (pTracerEffect)
		{
			FVector MuzzleLocation = RangeOwner->GetMesh()->GetSocketLocation(MuzzleSocketName);

			UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), pTracerEffect, MuzzleLocation);
			if (TracerComp)
			{
				TracerComp->SetVectorParameter(TracerTargetName, TracerEndPoint);
			}
		}
	}
}

bool AEnemyShotgunWeapon::MulticastFireExtras_Validate(FVector TracerEndPoint)
{
	return true;
}


void AEnemyShotgunWeapon::MulticastReloadSound_Implementation()
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), pReloadSound, GetActorLocation());
}

bool AEnemyShotgunWeapon::MulticastReloadSound_Validate()
{
	return true;
}