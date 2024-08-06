// Copyright Quarantine Games 2020


#include "EnemyRifleWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "..\Player/BasePlayer.h"
#include "Particles/ParticleSystemComponent.h"
#include "..\Interfaces/RaycastNotifyable.h"
#include "Net/UnrealNetwork.h"
#include "..\Player/BasePlayer.h"
#include "BaseAICharacter.h"
#include "BaseAICharacter_AIController.h"
#include "..\Components/RaycastComponent.h"
#include "RangeAI.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

AEnemyRifleWeapon::AEnemyRifleWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	fBaseDamage = 1.f;
	//InitialDamage = fBaseDamage;


	//fMaxAmmo = 1328.f;
	//fCurrentAmmo = fMaxAmmo;
	fClipSizeAmount = 32;
	fCurrentAmmo = fClipSizeAmount;

	fFireTimer = 0;
	fTimeBetweenShots = 0.2;
	fBulletSpread = 10;
	fWeaponRange = 20000.f;

	fItemLoudness = .32f;

	//SetReplicates(true);
	//SetReplicatingMovement(true);
}

void AEnemyRifleWeapon::BeginPlay()
{
	Super::BeginPlay();

	InitialDamage = fBaseDamage;
}

void AEnemyRifleWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemyRifleWeapon::FireAutoAI()
{
	if (fFireTimer <= 0)
	{
		fFireTimer = fTimeBetweenShots;
		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::SanitizeFloat(fCurrentAmmo));

		fCurrentAmmo--;

		if (fCurrentAmmo <= 0) //if ammo less then 0, reload
		{
			ABaseAICharacter* rangeAI = Cast<ABaseAICharacter>(GetOwner());
			if (rangeAI)
			{
				ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(rangeAI->GetController());
				if (controller)
				{
					controller->GetBlackboardComponent()->SetValueAsBool("Reloading", true);
					GetWorldTimerManager().SetTimer(TimerHandle_Reloading, this, &AEnemyRifleWeapon::ReloadTimer, 3.7f, false);

					MulticastReloadExtra();
					fCurrentAmmo = fClipSizeAmount;
				}
			}
		}

		AActor* MyOwner = GetOwner();
		if (MyOwner)
		{
			//ACharacter* Character = Cast<ACharacter>(MyOwner);

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
				ABasePlayer* basePlayer = Cast<ABasePlayer>(HitActor);
				if (basePlayer)
				{
					//basePlayer->MakeNoise(0.75f, nullptr, GetActorLocation()); //gets noise from weapon
					UGameplayStatics::ApplyPointDamage(HitActor, fBaseDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), MyOwner, DamageType);
					
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

	//ForceNetUpdate();
}

void AEnemyRifleWeapon::ReloadTimer()
{
	ABaseAICharacter* rangeAI = Cast<ABaseAICharacter>(GetOwner());
	if (rangeAI)
	{
		ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(rangeAI->GetController());
		if (controller)
		{
			controller->GetBlackboardComponent()->SetValueAsBool("Reloading", false);
		}
	}
}

void AEnemyRifleWeapon::MulticastFireExtras_Implementation(FVector TracerEndPoint)
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


void AEnemyRifleWeapon::MulticastReloadExtra_Implementation()
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), pReloadSound, GetActorLocation());
}