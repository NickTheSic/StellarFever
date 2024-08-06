// Copyright Quarantine Games 2020


#include "FragGrenadeAI.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystem.h"
#include "..\Player\BasePlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Materials/MaterialInterface.h"
#include "Components/DecalComponent.h"
#include "../StellarFeverGameModeBase.h"

// Sets default values
AFragGrenadeAI::AFragGrenadeAI()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	pCapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>("CapsuleComp");
	pCapsuleComponent->IgnoreActorWhenMoving(GetOwner(), true);
	RootComponent = pCapsuleComponent;

	pMesh = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	pMesh->IgnoreActorWhenMoving(GetOwner(), true);
	pMesh->SetupAttachment(RootComponent);

	pProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	pProjectileMovement->InitialSpeed = 1500.0f;
	pProjectileMovement->MaxSpeed = 1500.0f;
	pProjectileMovement->bShouldBounce = true;

	fExplosionDamage = 50;
	fExplosionRadius = 300.f;
	fExplodeTime = 1.5f;

	pSphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	pSphereComp->SetSphereRadius(fExplosionRadius);
	pSphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//pSphereComp->SetCollisionResponseToChannels(ECR_Ignore);
	//pSphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	pSphereComp->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore); //ask ryan
	pSphereComp->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Ignore); //ask ryan
	pSphereComp->SetVisibility(true);
	pSphereComp->bHiddenInGame = true;
	pSphereComp->SetupAttachment(RootComponent);

	//NetUpdateFrequency = 8; //FRICKIN IMPORTANT!


	//SetReplicates(true);
	//SetReplicateMovement(true);
}


// Called when the game starts or when spawned
void AFragGrenadeAI::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(TimerHandle_Explode, this, &AFragGrenadeAI::Explode, fExplodeTime, false);

	pCapsuleComponent->OnComponentHit.AddDynamic(this, &AFragGrenadeAI::OnHit);

	//UGameplayStatics::SpawnDecalAtLocation(GetWorld(), pRedRadius, FVector(320, 320, 320), GetActorLocation(), FRotator(0, 0, 0), 3.2f);
	
	//UGameplayStatics::SpawnDecalAttached(pRedRadius, FVector(370, 370, 370), pMesh, "", GetActorLocation(), FRotator(-90, 0.f, -90), EAttachLocation::KeepWorldPosition, 3.2f);
	
	AStellarFeverGameModeBase* pGameMode = Cast<AStellarFeverGameModeBase>(GetWorld()->GetAuthGameMode());
	if (pGameMode)
	{
		fExplosionDamage = fExplosionDamage + (1.f * pGameMode->GetDifficulyValue()); //they delete, thats why
	}
}

// Called every frame
void AFragGrenadeAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFragGrenadeAI::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	UGameplayStatics::SpawnDecalAttached(pGreenRadius, FVector(50, 370, 370), pMesh, "", GetActorLocation(), FRotator(-90, 0.f, -90), EAttachLocation::KeepWorldPosition, 1.2);

}

void AFragGrenadeAI::Explode()
{
	MulticastSelfDestruct();

	if (pSphereComp != nullptr)
	{
		TArray<AActor*> CollectedActors;
		pSphereComp->GetOverlappingActors(CollectedActors);

		for (auto& actor : CollectedActors)
		{
			//if (actor->ActorHasTag(FName("Pickup")))
			ABasePlayer* player = Cast<ABasePlayer>(actor);
			if (Cast<ABasePlayer>(player))
			{
				//Deplete all hit shield here
				player->DrainHitShield(5);
				//UGameplayStatics::ApplyDamage(actor, fExplosionDamage, GetController(), this, nullptr);
				UGameplayStatics::ApplyDamage(player, fExplosionDamage, nullptr, this, nullptr);
			}
		}
	}

	//ForceNetUpdate();

	//pNiagaraEffect->DestroyComponent();
	////pParticleSystem->DestroyComponent();
	//pNiagaraEffect->Activate(false);
	//pParticleSystem->Activate(false);
	//pParticleSystem->DeactivateSystem();
	//pNiagaraEffect->DestroyInstance();
	//pNiagaraEffect->Deactivate();
	//pNiagaraEffect->DeactivateImmediate();
	Destroy();
}

void AFragGrenadeAI::MulticastSelfDestruct_Implementation()
{
	if (pExplosionEffect)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), pExplosionEffect, GetActorLocation());
	if (pExplosionNiagara)
		pNiagaraEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, pExplosionNiagara, GetActorLocation(), FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f));

	UGameplayStatics::PlaySoundAtLocation(this, pExplodeSound, GetActorLocation());
}
bool AFragGrenadeAI::MulticastSelfDestruct_Validate()
{
	return true;
}

