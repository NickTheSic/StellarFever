// Copyright Quarantine Games 2020


#include "EngineerBullet.h"
#include "../../AI/AIEnemy.h"
#include "../../AI/EnemyShield.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/AudioComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "../../StellarFeverGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "EngineerGun.h"
#include "Sound/SoundCue.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

// Sets default values
AEngineerBullet::AEngineerBullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	pAIWeHitRef = nullptr;
	bAttachedToShield = false;
	fTimeBeforeExploding = 15.0f;
	ucShotOrder = 0;

	//this should be root component therefore when we change actor location it will move the mesh
	pMesh = CreateDefaultSubobject<UStaticMeshComponent>("Bullet Mesh");

	//Getting the Mesh via c++											 "classType '/Path/To/Model/Goes/Here' "
	const ConstructorHelpers::FObjectFinder<UStaticMesh> bulletMesh(TEXT("StaticMesh'/Game/ART_Spencer/Engineer_Bullet'"));
	if (bulletMesh.Succeeded()) pMesh->SetStaticMesh(bulletMesh.Object);

	pMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	pMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	pMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
	RootComponent = pMesh;

	pBulletCapsule = CreateDefaultSubobject<UCapsuleComponent>("Bullet Hitbox");
	pBulletCapsule->SetCollisionProfileName("NoCollision");
	pBulletCapsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	pBulletCapsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
	pBulletCapsule->SetupAttachment(RootComponent);

	pExplosionRadius = CreateDefaultSubobject<USphereComponent>("Explosion Radius");
	pExplosionRadius->SetCollisionProfileName("OverlapAll");
	pExplosionRadius->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	pExplosionRadius->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
	pExplosionRadius->SetSphereRadius(150.0f);
	pExplosionRadius->SetHiddenInGame(true); //for testing purposes
	pExplosionRadius->SetGenerateOverlapEvents(true);
	pExplosionRadius->SetupAttachment(RootComponent);

	//not attaching so the sound won't follow if we move the bullet
	pAudioComponent = CreateDefaultSubobject<UAudioComponent>("Audio Component");

	const ConstructorHelpers::FObjectFinder<USoundCue> explodeSound(TEXT("USoundCue'/Game/ART_Justin/EngineerSounds/EngineerBulletDetonate_Cue'"));
	if (explodeSound.Succeeded()) pAudioComponent->SetSound(explodeSound.Object);

	pAudioComponent->SetIsReplicated(true);

	bReplicates = true;
	SetReplicates(true);
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void AEngineerBullet::BeginPlay()
{
	Super::BeginPlay();

	pAudioComponent->SoundClassOverride = GetGameInstance<UStellarFeverGameInstance>()->pGeneralSoundClass;
	pAudioComponent->Stop(); //explosion sound automatically played on startup without this for some reason
}

// Called every frame
void AEngineerBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (pAIWeHitRef != nullptr)
	{
		//check if the enemy we're stuck to is dead, if so
		if (pAIWeHitRef->bDied == true)
		{
			//remove bullet (detaches from actor)
			Server_Explode(40.0f);
		}
		//if attached to a shield
		else if (bAttachedToShield)
		{
			//and shield exists
			if (pAIWeHitRef->pCurrentShield != nullptr)
			{
				//but is destroyed
				if (pAIWeHitRef->pCurrentShield->fShieldHealth <= 0)
				{
					//remove bullet (detaches from actor)
					Server_Explode(40.0f);
				}
			}
		}
	}
}

//using the ImpactPoint from hit, we can place and activate a bullet at the proper location and it should overlap with the actor it hits, prioritizing enemies
void AEngineerBullet::Server_Fire_Implementation(const FHitResult& Hit, const FQuat& dir, float BurnDamage, float BurnDuration, float HitsPerSecond)
{
	//setup max lifetime explosion timer
	GetWorld()->GetTimerManager().SetTimer(EndOfLifeExplosionTimer, this, &AEngineerBullet::EndOfLifeExplode, fTimeBeforeExploding, false);

	pExplosionRadius->SetCollisionProfileName("NoCollision");
	pExplosionRadius->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	pExplosionRadius->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);

	//place at raycast's ImpactPoint ("fakes" attachment to walls, will cause issues if there's moving parts)
	SetActorLocation(Hit.ImpactPoint);
	SetActorRotation(dir);
	//reveal bullet
	SetActorHiddenInGame(false);

	//if there's another actor
	if (Hit.GetActor() != nullptr)
	{
		//if the actor has tag "Enemy"
		if (Hit.GetActor()->ActorHasTag("Enemy"))
		{
			//cast to enemy and save enemy we hit
			pAIWeHitRef = static_cast<AAIEnemy*>(Hit.GetActor());

			//stick to that actor (our bullets follow the enemy we hit)
			AttachToActor(pAIWeHitRef, FAttachmentTransformRules::KeepWorldTransform);

			//deal damage					Gun			Engineer
			pAIWeHitRef->InitiateBurnDamage(GetOwner()->GetOwner(), BurnDamage, BurnDuration, HitsPerSecond);
		}
		else if (Hit.GetActor()->ActorHasTag("Shield"))
		{
			//cast to enemy and save enemy we hit
			pAIWeHitRef = Cast<AAIEnemy>(Hit.GetActor()->GetOwner());

			//stick to that actor (our bullets follow the enemy we hit)
			AttachToActor(Hit.GetActor(), FAttachmentTransformRules::KeepWorldTransform);

			bAttachedToShield = true;

			//deal damage					Gun			Engineer
			//pAIWeHitRef->InitiateBurnDamage(GetOwner()->GetOwner(), BurnDamage, BurnDuration, HitsPerSecond);
		}
		//if the actor has tag "Player"
		else if (Hit.GetActor()->ActorHasTag("Player"))
		{
			//stick to that actor (our bullets follow the player we hit)
			AttachToActor(Hit.GetActor(), FAttachmentTransformRules::KeepWorldTransform);
		}
		else
		{
			AttachToActor(Hit.GetActor(), FAttachmentTransformRules::KeepWorldTransform);
		}
	}
}

void AEngineerBullet::Server_Explode_Implementation(float ExplosionDamage)
{
	NetMC_PlayExplodeSound();
	NetMC_PlayWorldCameraShake();

	//this bullet has exploded and is no longer in the shot order
	SetShotOrder(0);
	//if we hit an ai, clear it
	if (pAIWeHitRef != nullptr)
		pAIWeHitRef = nullptr;

	bAttachedToShield = false;

	GetWorldTimerManager().ClearTimer(EndOfLifeExplosionTimer);

	pExplosionRadius->SetCollisionProfileName("OverlapAll");
	pExplosionRadius->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	pExplosionRadius->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);

	TArray<AActor*> overlappingActors;
	pExplosionRadius->GetOverlappingActors(overlappingActors);

	for (auto i : overlappingActors)
	{
		if (i->ActorHasTag("Enemy"))
		{
			FHitResult hit;
			//if we our raycast doesn't hit any geometry
			if (!(GetWorld()->LineTraceSingleByChannel(hit, GetActorLocation(), i->GetActorLocation(), ECollisionChannel::ECC_GameTraceChannel4)))
			{
				AAIEnemy* enemy = static_cast<AAIEnemy*>(i);

				//if attached to a shield, and enemy is the one we're attached to
				if (bAttachedToShield && enemy == pAIWeHitRef)
				{
					//damage his shield
					//if so, cast its shield and deal damage
					AEnemyShield* shield = Cast<AEnemyShield>(enemy->pCurrentShield);
					if (shield != nullptr && shield->fShieldHealth > 0.0f)
					{
						//one shot the shield
						shield->HurtShield(ExplosionDamage);
					}
					//otherwise hurt the AI
					else
					{
						//deal explosion damage							   Gun		   Engineer								  Gun		  Engineer								 Gun		  DamageType
						UGameplayStatics::ApplyDamage(i, ExplosionDamage, GetOwner()->GetOwner()->GetInstigatorController(), GetOwner()->GetOwner(), static_cast<AEngineerGun*>(GetOwner())->GetWeaponDamageType());
					}
				}
				//if not attached to a shield/handling a different enemy
				else
				{
					//if the enemy is a shield enemy
					if (enemy->bShieldEnemy)
					{
						//check if it has a shield
						if (enemy->pCurrentShield)
						{
							//if so, cast its shield and deal damage
							AEnemyShield* shield = Cast<AEnemyShield>(enemy->pCurrentShield);
							if (shield != nullptr && shield->fShieldHealth > 0.0f)
							{
								//damage the shield
								shield->HurtShield(ExplosionDamage);
							}
							//otherwise hurt the AI
							else
							{
								//deal explosion damage							   Gun		   Engineer								  Gun		  Engineer								 Gun		  DamageType
								UGameplayStatics::ApplyDamage(i, ExplosionDamage, GetOwner()->GetOwner()->GetInstigatorController(), GetOwner()->GetOwner(), static_cast<AEngineerGun*>(GetOwner())->GetWeaponDamageType());
							}
						}
					}
					//this enemy doesn't have a shield
					else
					{
						//deal explosion damage							   Gun		   Engineer								  Gun		  Engineer								 Gun		  DamageType
						UGameplayStatics::ApplyDamage(i, ExplosionDamage, GetOwner()->GetOwner()->GetInstigatorController(), GetOwner()->GetOwner(), static_cast<AEngineerGun*>(GetOwner())->GetWeaponDamageType());
					}
				}
			}
		}
			
	}
	//if attached
	if (GetAttachParentActor() != nullptr)
	{
		//Detach from actor
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}

	//move bullet to hidden location so we don't accidentally hit our bullet somehow
	SetActorLocation(FVector(0, 0, -20000));
}

void AEngineerBullet::NetMC_PlayExplodeSound_Implementation()
{
	//place at bullet's position, play explosion sound
	pAudioComponent->SetWorldLocation(GetActorLocation());
	pAudioComponent->Play();
}

void AEngineerBullet::NetMC_PlayWorldCameraShake_Implementation()
{
	UGameplayStatics::PlayWorldCameraShake(GetWorld(), cameraShake, GetActorLocation(), 0.0f, pExplosionRadius->GetUnscaledSphereRadius() * 10.0f, 1.0f, false);
}

void AEngineerBullet::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEngineerBullet, pAudioComponent);
}