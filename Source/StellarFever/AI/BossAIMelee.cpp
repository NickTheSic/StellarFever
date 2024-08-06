// Copyright Quarantine Games 2020


#include "BossAIMelee.h"
#include "BaseAICharacter_AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnemyMeleeWeapon.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "..\Components/HealthComponent.h"
#include "DrawDebugHelpers.h"
#include "..\Player\BasePlayer.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInterface.h"
#include "EnemyShield.h"
#include "Net/UnrealNetwork.h"

#include "BaseAICharacter_AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "..\Player\BasePlayer.h"
#include "..\Components/HealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimMontage.h"

ABossAIMelee::ABossAIMelee(FObjectInitializer const& object_initializer)
{
	PrimaryActorTick.bCanEverTick = true;

	pHealthComp->DefaultHealth = 470.f;
	InitialHealth = pHealthComp->DefaultHealth;

	fBelowHealthRun = 100.f;

	ProtectInfrontPosition = CreateDefaultSubobject<USceneComponent>("FollowPoint");
	ProtectInfrontPosition->SetupAttachment(GetRootComponent());
	ProtectInfrontPosition->ComponentTags.Add(FName("ProtectInfrontPosition"));

	bBossAttacker = true;

	DistancePlayer = 1000000.f;

	fAttackRangeDistance = 120.f;

	fAIMovementSpeed = 1000.f;
	InitialSpeed = fAIMovementSpeed;

	//Gravity = 420.000000f;
	//Velocity = 1620.0;

	Tags.Add("BossAI");
	Tags.Add("BossMelee");
}

void ABossAIMelee::BeginPlay()
{
	Super::BeginPlay();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	pBossMeleeWeapon = GetWorld()->SpawnActor<AEnemyMeleeWeapon>(StarterMeleeClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);


	if (GetLocalRole() == ROLE_Authority) //OOHHH REMEMBER make local authority so not more then 1 spawns
	{
		pCurrentShield = GetWorld()->SpawnActor<AEnemyShield>(StarterShieldClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (pCurrentShield)
		{
			pCurrentShield->SetOwner(this);
			pCurrentShield->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "ShieldSocket");
			pCurrentShield->fShieldHealth = 100.f;

		}
		if (pBossMeleeWeapon)
		{
			pBossMeleeWeapon->SetOwner(this);
			pBossMeleeWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "MeleeHitPointBoss");
		}
	}

	GetCharacterMovement()->MaxWalkSpeed = fOriginalWalkSpeed;
	//if (GetLocalRole() == ROLE_Authority)
	{
		//pHealthComp->OnHealthChanged.AddDynamic(this, &ABossAIMelee::OnHealthChanged);
	}
}

void ABossAIMelee::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(GetController());

	if (controller && !controller->GetBlackboardComponent()->GetValueAsBool("RunAway")) //RunAway = false
	{
		controller->GetBlackboardComponent()->SetValueAsBool("LungeWait", true); //ChasePlayer all times
	}
	if (controller && controller->GetBlackboardComponent()->GetValueAsBool("RunAway")) //RunAway = true
	{
		controller->GetBlackboardComponent()->SetValueAsBool("LungeWait", false);
	}

	// Include Health Code from BossAI If needed HERE
	if (pHealthComp->GetHealth() <= fBelowHealthRun && bRunToHeal == false && bResetHealActive == false && controller)
	{
		bResetHealActive = true;
		bRunToHeal = true;
		GetWorldTimerManager().SetTimer(TimerHandle_ResetHealActive, this, &ABossAIMelee::ResetHealActive, 30.0f, false);

		controller->GetBlackboardComponent()->SetValueAsBool("HealRun", true);
		
		ShockwaveAttack(); //Instant Showave attack
	}

	//Melee Attack Code
	//if (DistancePlayer < 230)
	if (DistancePlayer < 230)
	{
		AttackMelee();
		ShockwaveAttack();
	}

	//LUNGE TICK CODE
	if (DistancePlayer < 900 && bLungeAttack == false)
	{
		bLungeAttack = true;
		//GetWorldTimerManager().SetTimer(TimerHandle_LungeAttack, this, &ABossAIMelee::LungeAttack, 1.32f, true);
		GetWorldTimerManager().SetTimer(TimerHandle_LungeAttack, this, &ABossAIMelee::LungeAttack, .87f, true);
	}

	//STRAFE TICK CODE
	if (DistancePlayer < 900 && bStrafeAway == false)
	{
		bStrafeAway = true;
		//GetWorldTimerManager().SetTimer(TimerHandle_Strafe, this, &ABossAIMelee::StrafeFunction, 1.2f, true);
		GetWorldTimerManager().SetTimer(TimerHandle_Strafe, this, &ABossAIMelee::StrafeFunction, 1.f, true);

	}
	if (DistancePlayer > 2000 && bStrafeAway == true)
	{
		bStrafeAway = false;
		GetWorldTimerManager().ClearTimer(TimerHandle_Strafe);
	}


	//SPEED CODE (RUNAWAY)
	if (controller)
	{
		if (controller->GetBlackboardComponent()->GetValueAsBool("RunAway"))
		{
			GetCharacterMovement()->MaxWalkSpeed = 3600.f;
		}
		if (!controller->GetBlackboardComponent()->GetValueAsBool("RunAway"))
		{
			GetCharacterMovement()->MaxWalkSpeed = fOriginalWalkSpeed;
		}
	}


	if (pCurrentShield && pCurrentShield->fShieldHealth <= 0 && bShieldRestart == false)
	{
		bShieldRestart = true;
		GetWorldTimerManager().SetTimer(TimerHandle_ShieldRestart, this, &ABossAIMelee::EnableShield, 8.f, false);
	}


	//Shockwave Periodically (Maybe change)
	/*if (DistancePlayer <= 400 && bShockWaveFlag == false)
	{
		bShockWaveFlag = true;
		GetWorldTimerManager().SetTimer(TimerHandle_ShockWave, this, &ABossAIMelee::ShockwaveAttack, fShockwaveDelay, true);
	}
	if (DistancePlayer > 400 && bShockWaveFlag == true)
	{
		bShockWaveFlag = false;
		GetWorldTimerManager().ClearTimer(TimerHandle_ShockWave);
	}*/

	//ADD GRENADE CODE? From BossAI
}

void ABossAIMelee::OnHealthChangedInternal(UHealthComponent* InHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType2, class AController* InstigatedBy, AActor* DamageCauser)
{
	Super::OnHealthChangedInternal(InHealthComp, Health, HealthDelta, DamageType2, InstigatedBy, DamageCauser);

	//ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(GetController());

	//bool HealRun = controller->GetBlackboardComponent()->GetValueAsBool("HealRun");

	//if (HealRun || bStunned)
	//{
	//	return;
	//}

	//if (controller && DistancePlayer < 1100)
	//{
	//	//controller->GetBlackboardComponent()->S etValueAsBool("RunAway", true);
	//	FVector velocity = (GetActorRightVector());
	//	velocity.Z += .17;
	//	velocity.Normalize();

	//	int random = FMath::RandRange(0, 1);
	//	if (random == 0) //left
	//	{
	//		LaunchCharacter(velocity * 2000, false, true);
	//	}
	//	if (random == 1) //right
	//	{
	//		LaunchCharacter(-velocity * 2000, false, true);
	//	}
	//}
}

void ABossAIMelee::HealSelfInitialize()
{
	iHealIndex = 0;
	GetCharacterMovement()->MaxWalkSpeed = 0.f;

	UHealthComponent* healthComp = Cast<UHealthComponent>(GetComponentByClass(UHealthComponent::StaticClass()));
	if (healthComp)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_HealSelf, this, &ABossAIMelee::HealSelfTick, fHealTickInterval, true);
		EnableShield();
	}
}

void ABossAIMelee::HealSelfTick()
{
	if (bResetHealActive == false)
	{
		return;
	}

	UHealthComponent* healthComp = Cast<UHealthComponent>(GetComponentByClass(UHealthComponent::StaticClass()));
	if (healthComp)
	{
		iHealIndex++;
		healthComp->Heal(fHealPerTick);
	}
	if (iHealIndex >= 5)
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_HealSelf);
		GetCharacterMovement()->MaxWalkSpeed = fOriginalWalkSpeed;

		ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(GetController());
		iHealIndex = 0;

		bRunToHeal = false;
		//bGreanadePlay = false;

		if (controller)
		{
			controller->GetBlackboardComponent()->SetValueAsBool("CanSeePlayer", true);
			controller->GetBlackboardComponent()->SetValueAsBool("Patrol", false);
			FindNextTarget();
		}
	}
}

void ABossAIMelee::ResetHealActive()
{
	bResetHealActive = false;
}


void ABossAIMelee::AttackMelee()
{
	if (!bStunned)
	{
		if (pBossMeleeWeapon && bRunToHeal == false)
		{
			pBossMeleeWeapon->FireSingleAI();
		}
	}
}

void ABossAIMelee::ShockwaveAttack()
{
	ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(GetController());
	if (controller)
	{
		bool HealRun = controller->GetBlackboardComponent()->GetValueAsBool("HealRun");

		if (HealRun || bStunned)
		{
			return;
		}
	}

	TArray<FHitResult> OutHits;

	FVector MyLocation = GetActorLocation();

	FCollisionShape MyColSphere = FCollisionShape::MakeSphere(500.f);

	//DrawDebugSphere(GetWorld(), MyLocation, MyColSphere.GetSphereRadius(), 50, FColor::Cyan, false, 1.f);

	//bool isHit = GetWorld()->SweepMultiByChannel(OutHits, MyLocation, MyLocation, FQuat::Identity, ECC_WorldStatic, MyColSphere);
	bool isHit = GetWorld()->SweepMultiByChannel(OutHits, MyLocation, MyLocation, FQuat::Identity, ECC_Pawn, MyColSphere);

	if (isHit)
	{
		for (auto& Hit : OutHits)
		{
			ABasePlayer* Player = Cast<ABasePlayer>((Hit.GetActor()));

			if (Player && bShockwaveEnable == false)
			{
				bShockwaveEnable = true;

				FVector velocity = (Player->GetActorLocation() - GetActorLocation());
				velocity.Z += 88.f;
				velocity.Normalize();
				
				//UGameplayStatics::ApplyDamage(Player, 25, GetInstigatorController(), this, DamageType);
				Player->LaunchCharacter(velocity * 1220.f, true, true);
				bShockwaveEnable = false;
			}
		}
	}

	//FVector feetPosition = GetActorLocation();
	//feetPosition.Z -= 111.f;
	//UGameplayStatics::SpawnDecalAttached(pCyanRadius, FVector(50, 590, 590), GetMesh(), "", feetPosition, FRotator(-90, 0.f, -90), EAttachLocation::KeepWorldPosition, .5f);
	MulticastShockwave();
}


void ABossAIMelee::MulticastShockwave_Implementation()
{
	FVector feetPosition = GetActorLocation();
	feetPosition.Z -= 111.f;

	//Shows shockwave displayer for short second
	UGameplayStatics::SpawnDecalAttached(pCyanRadius, FVector(50, 590, 590), GetMesh(), "", feetPosition, FRotator(-90, 0.f, -90), EAttachLocation::KeepWorldPosition, .5f);
}

void ABossAIMelee::EnableShield()
{
	bShieldRestart = false;

	if (pCurrentShield)
	{
		pCurrentShield->Enable(); //keep at top cuz of shield health
		pCurrentShield->fShieldHealth = 100.f;
		/*pCurrentShield->SetActorHiddenInGame(false);
		pCurrentShield->SetActorEnableCollision(true);
		pCurrentShield->SetActorTickEnabled(true);*/
	}
}
void ABossAIMelee::DisableShield()
{
	if (pCurrentShield)
	{
		pCurrentShield->Disable();
		/*pCurrentShield->SetActorHiddenInGame(true);
		pCurrentShield->SetActorEnableCollision(false);
		pCurrentShield->SetActorTickEnabled(false);*/
	}
}

void ABossAIMelee::LungeAttack()
{
	bLungeAttack = false;
	GetWorldTimerManager().ClearTimer(TimerHandle_LungeAttack);

	ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(GetController());

	controller->GetBlackboardComponent()->SetValueAsBool("RunAway", false);


	if (controller)
	{
		bool HealRun = controller->GetBlackboardComponent()->GetValueAsBool("HealRun");

		if (HealRun || bStunned)
		{
			return;
		}
	}
	if (controller)
	{
		//controller->GetBlackboardComponent()->SetValueAsBool("LungeAttack", true);

		UObject* player = controller->GetBlackboardComponent()->GetValueAsObject("TargetActor");
		ABasePlayer* Player = Cast<ABasePlayer>(player);

		if (Player && Player->GetCharacterMovement()->IsMovingOnGround())
		{
			FVector velocity = (Player->GetActorLocation() - GetActorLocation());
			//FVector velocity = (GetActorForwardVector());
			//velocity.Z += 0.1;
			//velocity.X -= 0.15;
			velocity.Z += 100;
			//velocity.X -= 32;
			velocity.Normalize();
			velocity *= 2300;

			//GetCharacterMovement()->SetMovementMode(MOVE_Falling);

			LaunchCharacter(velocity, false, false);
			//GetCharacterMovement()->Launch(velocity);
			//GetCharacterMovement()->AddImpulse(velocity, true);
			//GetCharacterMovement()->AddForce(velocity);

		}
	}

	//FTimerHandle TimerHandle_ShieldEnableAfterLunge;
	//GetWorldTimerManager().SetTimer(TimerHandle_ShieldEnableAfterLunge, this, &ABossAIMelee::EnableShield, 1.1f, false);

}

void ABossAIMelee::StrafeFunction()
{
	ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(GetController());

	bool HealRun = controller->GetBlackboardComponent()->GetValueAsBool("HealRun");

	if (HealRun || bStunned)
	{
		return;
	}

	if (controller && DistancePlayer < 1100)
	{
		//controller->GetBlackboardComponent()->S etValueAsBool("RunAway", true);
		FVector velocity = (GetActorRightVector());
		velocity.Z += .17;
		velocity.Normalize();

		int random = FMath::RandRange(0, 1);
		if (random == 0) //left
		{
			LaunchCharacter(velocity * 1700, false, true);
		}
		if (random == 1) //right
		{
			LaunchCharacter(-velocity * 1700, false, true);
		}
	}
}
