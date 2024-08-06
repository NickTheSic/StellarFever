// Copyright Quarantine Games 2020


#include "EnemyMeleeWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "BaseAICharacter.h"
#include "DrawDebugHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Engine/Engine.h"
#include "Net/UnrealNetwork.h"
#include "../Player/BasePlayer.h"
#include "MeleeAI2.h"
#include "BaseAICharacter_AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BossAIMelee.h"
#include "ShieldAI.h"
#include "EnemyShield.h"

AEnemyMeleeWeapon::AEnemyMeleeWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	
	fBaseDamage = 10.f;
	//InitialDamage = fBaseDamage;

	bAttackReset = true;

	//MaxAmmo = 328.f;
	//CurrentAmmo = MaxAmmo;

	fFireTimer = 0;

	fMeleeAttackActive = 0.16f;
	fTimeBetweenShots = 2.3f;
	
	fBulletSpread = 0;
	fWeaponRange = 1520;

	fItemLoudness = .1;

	fAttackRange = 120.f;
}

void AEnemyMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();

	InitialDamage = fBaseDamage;

}

void AEnemyMeleeWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MeleeSocketCollision();
}

void AEnemyMeleeWeapon::FireSingleAI()
{
	if (fFireTimer <= 0 && bAttackReset)
	{

		//bMeleeAttackRange = true;
		bAttackReset = false;
		
		MulticastSwing();

		FTimerHandle TimerHandle_AttackSwingNoise;
		GetWorldTimerManager().SetTimer(TimerHandle_AttackSwingNoise, this, &AEnemyMeleeWeapon::MulticastSwingSound, .14f, false);

		FTimerHandle TimerHandle_ActualAttack;
		GetWorldTimerManager().SetTimer(TimerHandle_ActualAttack, this, &AEnemyMeleeWeapon::ActuallyAttack, .18f, false);

		//GetWorldTimerManager().SetTimer(TimerHandle_SwingDuration, this, &AEnemyMeleeWeapon::StopSwing, fMeleeAttackActive, false); //.5
		GetWorldTimerManager().SetTimer(TimerHandle_AttackReset, this, &AEnemyMeleeWeapon::ResetAttack, fTimeBetweenShots, false); //2.3f
	}
}

void AEnemyMeleeWeapon::StopSwing()
{
	bMeleeAttackRange = false;
	bMeleeHit = false; //wont matter cuz collision is off now
}

void AEnemyMeleeWeapon::ResetAttack()
{
	bAttackReset = true;
}

void AEnemyMeleeWeapon::ActuallyAttack()
{
	GetWorldTimerManager().SetTimer(TimerHandle_SwingDuration, this, &AEnemyMeleeWeapon::StopSwing, fMeleeAttackActive, false); //0.16f FAST
	bMeleeAttackRange = true;

	AMeleeAI2* melee = Cast<AMeleeAI2>(GetOwner()); //Both shield and melee?
	if (melee)
	{
		ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(melee->GetController());
		if (controller)
		{
			controller->GetBlackboardComponent()->SetValueAsBool("RunAway", true);
		}
	}

	AShieldAI* shieldAI2 = Cast<AShieldAI>(GetOwner()); //Both shield and melee?
	if (shieldAI2)
	{	
		if (shieldAI2->pCurrentShield && shieldAI2->pCurrentShield->bDeactive)
		{
			ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(shieldAI2->GetController());
			if (controller)
			{
				controller->GetBlackboardComponent()->SetValueAsBool("RunAway", true);
			}
		}
	}
}

void AEnemyMeleeWeapon::MeleeSocketCollision()
{
	AActor* MyOwner = GetOwner();
	ABaseAICharacter* BaseAI = Cast<ABaseAICharacter>(MyOwner);

	if (!MyOwner || !BaseAI)
	{
		return;
	}

	// We're doing getmesh, NOT staticmesh
	const USkeletalMeshSocket* WeaponSocket = BaseAI->GetMesh()->GetSocketByName(FName("MeleeHitPoint"));
	if (WeaponSocket && bMeleeAttackRange)
	{
		//FHitResult Hit;
		TArray<FHitResult> Hits;

		FCollisionQueryParams CollisionParams;
		FCollisionObjectQueryParams ObjectQueryParams;

		//CollisionParams.AddIgnoredActor(this);
		//CollisionParams.AddIgnoredActor(MyOwner);

		//FCollisionShape MyColSphere = FCollisionShape::MakeSphere(60.f);
		FCollisionShape MyColSphere = FCollisionShape::MakeSphere(fAttackRange);
		//DrawDebugSphere(GetWorld(), WeaponSocket->GetSocketLocation(BaseAI->GetMesh()), MyColSphere.GetSphereRadius(), 32, FColor::Red, false, 0.1);

		//if (GetWorld()->SweepSingleByChannel(Hit, WeaponSocket->GetSocketLocation(BaseAI->GetMesh()), WeaponSocket->GetSocketLocation(BaseAI->GetMesh()), FQuat::Identity, ECC_MAX, MyColSphere, CollisionParams))
		if (GetWorld()->SweepMultiByChannel(Hits, BaseAI->GetMesh()->GetSocketLocation("MeleeHitPoint"), BaseAI->GetMesh()->GetSocketLocation("MeleeHitPoint") * GetActorForwardVector() * 45, FQuat::Identity, ECC_MAX, MyColSphere, CollisionParams))
		{
			//AActor* HitActor = Hit.GetActor();
			
			for (FHitResult hit : Hits)
			{
				ABasePlayer* BaseActor = Cast<ABasePlayer>(hit.GetActor());
				//DrawDebugSphere(GetWorld(), WeaponSocket->GetSocketLocation(BaseAI->GetMesh()), MyColSphere.GetSphereRadius(), 32, FColor::Red, false, 0.1);
				if (BaseActor && !bMeleeHit)
				{
					UGameplayStatics::ApplyDamage(BaseActor, fBaseDamage, GetInstigatorController(), this, DamageType);
					//MulticastFireExtras();
					MulticastHitSound();
					//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Blue, "Human hit!!");
					bMeleeHit = true;

				}
			}
			//test

		}
	}
}

void AEnemyMeleeWeapon::MulticastFireExtras_Implementation()
{
	//AActor* MyOwner = GetOwner();
	//ABaseAICharacter* BaseAI = Cast<ABaseAICharacter>(MyOwner);

	//const USkeletalMeshSocket* WeaponSocket = BaseAI->GetMesh()->GetSocketByName(FName("MeleeHitPoint"));

	//FCollisionShape MyColSphere = FCollisionShape::MakeSphere(fAttackRange);
	////Use to test debug sphere good
	//DrawDebugSphere(GetWorld(), WeaponSocket->GetSocketLocation(BaseAI->GetMesh()), MyColSphere.GetSphereRadius(), 32, FColor::Red, false, 0.1);

}

bool AEnemyMeleeWeapon::MulticastFireExtras_Validate()
{
	return false;
}


void AEnemyMeleeWeapon::MulticastSwing_Implementation()
{

	AMeleeAI2* melee = Cast<AMeleeAI2>(GetOwner());
	if (melee)
	{
		if (melee->Attack1 && melee->Attack2)
		{
			int randAttack = FMath::RandRange(0, 1);
			UAnimInstance* AnimInstance = melee->GetMesh()->GetAnimInstance();

			if (randAttack == 0)
			{
				AnimInstance->Montage_Play(melee->Attack1, 1.f);
			}
			if (randAttack == 1)
			{
				AnimInstance->Montage_Play(melee->Attack2, 1.f);
			}
		}
	}

	ABossAIMelee* bossMelee = Cast<ABossAIMelee>(GetOwner());
	if (bossMelee)
	{
		if (bossMelee->Attack1 && bossMelee->Attack2)
		{
			int randAttack = FMath::RandRange(0, 1);
			UAnimInstance* AnimInstance = bossMelee->GetMesh()->GetAnimInstance();

			if (randAttack == 0)
			{
				AnimInstance->Montage_Play(bossMelee->Attack1, 1.f);
			}
			if (randAttack == 1)
			{
				AnimInstance->Montage_Play(bossMelee->Attack2, 1.f);
			}
		}

		//ABaseAICharacter_AIController* controller2 = Cast<ABaseAICharacter_AIController>(bossMelee->GetController());
		//if (controller2)
		//{
		//	//controller2->GetBlackboardComponent()->SetValueAsBool("RunAway", true);
		//}
	}

	AShieldAI* shieldAI = Cast<AShieldAI>(GetOwner());
	if (shieldAI)
	{
		if (shieldAI->Attack1 && shieldAI->Attack2)
		{
			int randAttack = FMath::RandRange(0, 1);
			UAnimInstance* AnimInstance = shieldAI->GetMesh()->GetAnimInstance();

			if (randAttack == 0)
			{
				AnimInstance->Montage_Play(shieldAI->Attack1, 1.f);
			}
			if (randAttack == 1)
			{
				AnimInstance->Montage_Play(shieldAI->Attack2, 1.f);
			}
		}
	}
}

void AEnemyMeleeWeapon::MulticastSwingSound_Implementation()
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), pFireSound, GetActorLocation());
}

void AEnemyMeleeWeapon::MulticastHitSound_Implementation()
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), pMeleeHitSound, GetActorLocation());
}

bool AEnemyMeleeWeapon::MulticastHitSound_Validate()
{
	return true;
}

