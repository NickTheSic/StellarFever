// Copyright Quarantine Games 2020


#include "AIEnemy.h"
#include "..\Components/HealthComponent.h"
#include "Materials/Material.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "..\Drops/DropsBase.h"
#include "Templates/SubclassOf.h"
#include "../StellarFeverGameModeBase.h"
#include "WeaponAI.h"
#include "EnemyShield.h"
#include "PatrolPath.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "UObject/ConstructorHelpers.h"
#include "EnemyMeleeWeapon.h"
#include "Components/CapsuleComponent.h"
#include "BaseAICharacter_AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "..\Ship parts\ShipDoor.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
AAIEnemy::AAIEnemy()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bDied = false;

	pHealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComp"));
	pHealthComp->fDefaultShieldAmount = 0.f;
	pHealthComp->fShieldAmount = 0.f;

	//pMesh2 = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("NewMeshComp"));
	//pMesh2->SetupAttachment(RootComponent);

	//Burn Damage code
	fBurnDamageDelay = 0.5f;
	fBurnDamageDuration = 4.f;
	fBurnDamageAmount = 5.f;

	//Stun Damage code
	fStunnedTime = 6.f;
	bStunned = false;

	//bAlwaysRelevant = true; //dunno?
	SetReplicates(true);
	SetReplicateMovement(true);

	NetUpdateFrequency = 40; //FRICKIN IMPORTANT!
	//ForceNetUpdate(); //USE OFTEN! Lower frequency on ALL AIS GETS CALLED next frame auto!
	//FlushNetDormancy(); Use only when Dormant is on initial or on Dormant

	fSpeedIncrease = 1.f;
	fRangeIncrease = 1.f;
	fHealthIncrease = 1.f;

	bDisabled = false;
	
	DeactivateSpot = GetActorLocation();//meant for Exploders
	
	this->Tags.Add("Enemy");
}

// Called when the game starts or when spawned
void AAIEnemy::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "SecurePath", SecurePaths); //ALWAYS add tag in SecurePaths!

	Activate();

	if (!bShipPatroller && !bShipBoarded) //Set ChaseDoor chance to AI - Scavenger dont ChaseDoors
	{
		int chanceOfDoorChase = FMath::RandRange(0, 3);
		if (chanceOfDoorChase)
		{
			float randomChanceTimer = FMath::FRandRange(5, 32);
		}
	} //Switch bChaseDoor to give em a chance to shut doors


	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "Door", pGetAllDoors);

	if (bGetDoors)
	{
		bEnableDoorMovement = true;
	}

	if (GetLocalRole() == ROLE_Authority)
	{
		pHealthComp->OnHealthChanged.AddDynamic(this, &AAIEnemy::OnHealthChanged);
	}

	//InitialHealth = pHealthComp->DefaultHealth;

	//TEST STUNNED() for Kentyn
	//FTimerHandle TimerHandle_BeginStun;
	//GetWorldTimerManager().SetTimer(TimerHandle_BeginStun, this, &AAIEnemy::Stunned, 5.f, false);
}

// Called every frame
void AAIEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bStunned)
	{
		GetCharacterMovement()->StopMovementImmediately();
		//controller->ClearFocus(EAIFocusPriority::Gameplay); //DUNNO
	}

	if (!bExploder) //Exploders remain at low health
	{
		pHealthComp->DefaultHealth *= fHealthIncrease; //Health Difficulty increase
	}
}

void AAIEnemy::OnHealthChanged(UHealthComponent* InHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType2, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0 && !bDied)
	{
		MulticastExplosion();
	}
	OnHealthChangedInternal(InHealthComp, Health, HealthDelta, DamageType2, InstigatedBy, DamageCauser);
}

void AAIEnemy::OnHealthChangedInternal(UHealthComponent* InHealthComp, float Health, float HealthDelta, const UDamageType* DamageType2, AController* InstigatedBy, AActor* DamageCauser)
{
	//LEAVE EMPTY
}

void AAIEnemy::MulticastExplosion_Implementation()
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), pDeathSounds, GetActorLocation());

	if (pDeathExplosionEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), pDeathExplosionEffect, GetActorLocation());
	}
}

void AAIEnemy::InitiateBurnDamage(AActor* DamageCauser, float DamageValue, float BurnEntireDuration, float TimesPerSecond)
{
	TimerDelegate_BurnDamage.BindUFunction(this, FName("BurnDamage"), DamageCauser, DamageValue);

	GetWorldTimerManager().SetTimer(TimerHandle_BurnDamage, TimerDelegate_BurnDamage, 1 / TimesPerSecond, true);

	FTimerHandle TimerHandle_TurnOffBurning;
	GetWorldTimerManager().SetTimer(TimerHandle_TurnOffBurning, this, &AAIEnemy::EndBurnDamage, BurnEntireDuration, false);

	MulticastBurnEffect();

	if (pFireParticleSystem)
	{
	}


	bEnableFlames = true;
}

void AAIEnemy::MulticastBurnEffect_Implementation()
{
	pFireParticleSystem = UGameplayStatics::SpawnEmitterAttached(pOnFireParticles, GetMesh());

	//pNiagaraFireEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, pFireNiagara, GetActorLocation(), FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f));
	//pNiagaraFireEffect = UNiagaraFunctionLibrary::SpawnSystemAttached(pFireNiagara, GetMesh(), "", GetActorLocation(), GetActorRotation(), EAttachLocation::SnapToTarget, false, true, ENCPoolMethod::AutoRelease);
	pNiagaraFireEffect = UNiagaraFunctionLibrary::SpawnSystemAttached(pFireNiagara, GetMesh(), "Marine002Pelvis", GetActorLocation(), GetActorRotation(), FVector(.2f, .2f, 3.f), EAttachLocation::KeepWorldPosition, false, ENCPoolMethod::AutoRelease);
}
bool AAIEnemy::MulticastBurnEffect_Validate()
{
	return true;
}

void AAIEnemy::BurnDamage(AActor* DamageCauser, float DamageValue)
{
	UGameplayStatics::ApplyDamage(this, DamageValue, DamageCauser->GetInstigatorController(), DamageCauser, DamageType);

	float HealthShow = pHealthComp->GetHealth();
	UE_LOG(LogTemp, Log, TEXT("Health Changed: %s"), *FString::SanitizeFloat(HealthShow));
}

void AAIEnemy::EndBurnDamage()
{
	bEnableFlames = false;

	if (pNiagaraFireEffect)
	{
		pNiagaraFireEffect->DeactivateImmediate();
	}
	
	if (pFireParticleSystem)
	{
		pFireParticleSystem->DestroyComponent();
	}

	GetWorldTimerManager().ClearTimer(TimerHandle_BurnDamage);
}

APatrolPath* AAIEnemy::get_patrol_path()
{
	return patrol_path;
}

void AAIEnemy::Stunned()
{
	bStunned = true;

	GetWorldTimerManager().SetTimer(TimerHandle_Stunned, this, &AAIEnemy::StunnedRelease, fStunnedTime, false);

	GetCharacterMovement()->MaxWalkSpeed = 0.f;
	MulticastStunEffectOn();
}

void AAIEnemy::MulticastStunEffectOn_Implementation()
{
	GetMesh()->SetMaterial(0, pStunnedMat);
	GetMesh()->SetMaterial(1, pStunnedMat);
	GetMesh()->SetMaterial(2, pStunnedMat);
	GetMesh()->SetMaterial(3, pStunnedMat);
	GetMesh()->SetMaterial(4, pStunnedMat);
	GetMesh()->SetMaterial(5, pStunnedMat);
}

void AAIEnemy::StunnedRelease()
{
	bStunned = false;

	GetWorldTimerManager().ClearTimer(TimerHandle_Stunned);

	GetCharacterMovement()->MaxWalkSpeed = fOriginalWalkSpeed;
	//GetMesh()->SetMaterial(0, pRegularMat);
	//GetMesh()->SetMaterial(1, pSecondMat);
	MulticastStunEffectOff();
}

void AAIEnemy::MulticastStunEffectOff_Implementation()
{
	GetMesh()->SetMaterial(0, pMat1);
	GetMesh()->SetMaterial(1, pMat2);
	GetMesh()->SetMaterial(2, pMat3);
	GetMesh()->SetMaterial(3, pMat4);
	GetMesh()->SetMaterial(4, pMat5);
	GetMesh()->SetMaterial(5, pMat6);
}

void AAIEnemy::DropItem()
{
	int random = FMath::RandRange(0, 2); //33 percent to drop item

	if (random == 0)
	{
		if (DropItemsArray.Num() > 0)
		{
			int itemRandom = FMath::RandRange(0, DropItemsArray.Num() - 1);

			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			ADropsBase* pDropItem = GetWorld()->SpawnActor<ADropsBase>(DropItemsArray[itemRandom], GetActorLocation(), GetActorRotation(), SpawnParams);
		}
	}

}

bool AAIEnemy::GetIsActive()
{
	return bIsActive;
}

void AAIEnemy::Activate()
{
	ChooseRandomAction(); //test

	Reset();
	bDisabled = false;
	bIsActive = true; //Robs code
	bReset = false;

	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);

	if (!bExploder)
	{
	}
	if (bExploder)
	{
		patrol_path = Cast<APatrolPath>(SecurePaths[0]);
	}
	bExploded = false;

	if (pCurrentWeapon)
	{
		pCurrentWeapon->Enable();
	}
	if (pBossMeleeWeapon)
	{
		pBossMeleeWeapon->Enable();
	}

	if (bShieldEnemy)
	{
		//fAIMovementSpeed = 500.f; //Regular speed for ShieldAI
		if (pCurrentShield)
		{
			pCurrentShield->bDeactive = false;
			pCurrentShield->Enable();
		}
	}


	if (bBossAttacker && !bShipBoarded)
	{
		//Chase Players
		bChasePlayers = true;
		bShipBoarded = false;
		bShipPatroller = false;

		ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(GetController());
		if (controller)
		{
			controller->GetBlackboardComponent()->SetValueAsBool("CanSeePlayer", true);
			controller->GetBlackboardComponent()->SetValueAsBool("Patrol", false);
			FindNextTarget();
		}
	}
	if (bBossAttacker && bShipBoarded)
	{
		bChasePlayers = false;
		bShipPatroller = false;

	}

	//Stats given
	AStellarFeverGameModeBase* pGameMode = Cast<AStellarFeverGameModeBase>(GetWorld()->GetAuthGameMode());
	if (pGameMode)
	{
		fAIMovementSpeed = InitialSpeed + (15.f * pGameMode->GetDifficulyValue());

		if (!bExploder) //Exploders do NOT increase in health
		{
			pHealthComp->DefaultHealth = InitialHealth + (20.f * pGameMode->GetDifficulyValue());
			pHealthComp->Health = pHealthComp->DefaultHealth;
		}
	}
}

void AAIEnemy::Deactivate()
{
	bDisabled = true;
	bIsActive = false; 

	SetActorHiddenInGame(true);
	int randomX = FMath::FRandRange(0, 1000); //50 percent to drop item
	int randomY = FMath::FRandRange(0, 1000);
	SetActorLocation(FVector(randomX, randomY, -9000.f));
	SetActorTickEnabled(false);

	if (!bExploder)
	{
	}
	if (bExploder)
	{
	}

	if (pCurrentWeapon)
	{
		pCurrentWeapon->Disable();
	}
	if (pBossMeleeWeapon)
	{
		pBossMeleeWeapon->Disable();
	}
	if (pCurrentShield)
	{
		pCurrentShield->Disable();
	}
}

void AAIEnemy::Reset()
{
	pHealthComp->Health = pHealthComp->DefaultHealth;
	//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::SanitizeFloat(pHealthComp->Health));
	pHealthComp->bIsDead = false;
	bDied = false;
	bGrenadePlay = false;
}


void AAIEnemy::ShipBoardedActivate()
{
	int randomMode = FMath::RandRange(0, 1);

	if (randomMode == 0) ////Stand Still
	{
		bShipBoarded = true;
		bChasePlayers = false;
		bShipPatroller = false;
	}
	if (randomMode == 1) ////Chase Players
	{
		bChasePlayers = true;
		bShipBoarded = false;
		bShipPatroller = false;
	}
}
void AAIEnemy::ChasePlayerActivate()
{
	//Chase Players
	bChasePlayers = true;
	bShipBoarded = false;
	bShipPatroller = false;

	ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(GetController());
	if (controller)
	{
		controller->GetBlackboardComponent()->SetValueAsBool("CanSeePlayer", true);
		controller->GetBlackboardComponent()->SetValueAsBool("Patrol", false);
		FindNextTarget();
	}
}
void AAIEnemy::ChaseDoorsActivate()
{
	bGetDoors = true;
	bEnableDoorMovement = true;

	bShipBoarded = false; //already on our ship
	bChasePlayers = false;
	bShipPatroller = false;
}

void AAIEnemy::ShipBoardedScavengersActivate()
{
	//Stand Still
	bShipBoarded = true;
	bChasePlayers = false;
	bShipPatroller = false;

	bScavengerBoardedEnemyShip = true; //Inside scavenger code
}

void AAIEnemy::ChooseRandomAction()
{
	int randomMode = FMath::RandRange(0, 2);

		if (!bBossAttacker)
		{
			if (!bShipBoarded) //thank God
			{
				if (randomMode == 0) //Chase Players
				{
					bChasePlayers = true;
					bShipBoarded = false;
					bShipPatroller = false;
				}
				if (randomMode == 1) //Chase Events
				{
					bChasePlayers = false;
					bShipBoarded = false;
					bShipPatroller = false;
				}
				if (randomMode == 2 && SecurePaths.Num() > 0) //Secure
				{


					bChasePlayers = false;
					bShipBoarded = false;
					bShipPatroller = true;
					patrol_path = Cast<APatrolPath>(SecurePaths[0]);
				}
				if (randomMode == 2 && SecurePaths.Num() == 0) //Chase Player
				{
					bChasePlayers = true;
					bShipBoarded = false;
					bShipPatroller = false;
				}
			}
		}
}

void AAIEnemy::FindNextTarget()
{
}

void AAIEnemy::FindRandomDoor()
{
}


// Called to bind functionality to input
void AAIEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AAIEnemy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(AAIEnemy, pRegularMat);
	DOREPLIFETIME(AAIEnemy, pCurrentShield);
}