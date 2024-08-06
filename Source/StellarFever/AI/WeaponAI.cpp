// Copyright Quarantine Games 2020


#include "WeaponAI.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/Engine.h"
#include "Sound/SoundBase.h"
#include "Net/UnrealNetwork.h"
#include "../StellarFeverGameModeBase.h"

// Sets default values
AWeaponAI::AWeaponAI()
{
 //	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "Target";

	pMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = pMeshComp;
	//MeshComp->bCastDynamicShadow = false;
	//MeshComp->CastShadow = false;

	bFireAnim = false;

	NetUpdateFrequency = 5; //FRICKIN IMPORTANT!

	SetReplicates(true);
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void AWeaponAI::BeginPlay()
{
	Super::BeginPlay();
}

// Called every fSrame
void AWeaponAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	fFireTimer -= DeltaTime;
}

void AWeaponAI::FireSingleAI()
{
	//ForceNetUpdate();
}

void AWeaponAI::FireAutoAI()
{
	//ForceNetUpdate();
}

void AWeaponAI::Enable()
{
	SetActorHiddenInGame(false);

	SetActorEnableCollision(true);

	SetActorTickEnabled(true);

	AStellarFeverGameModeBase* pGameMode = Cast<AStellarFeverGameModeBase>(GetWorld()->GetAuthGameMode());
	if (pGameMode)
	{
		fBaseDamage = InitialDamage + (1.f * pGameMode->GetDifficulyValue());

		//fTimeBetweenShots -= 0.1f * pGameMode->GetDifficulyValue();

		//pHealthComp->DefaultHealth = InitialHealth + (20.f * pGameMode->GetDifficulyValue());
		//pHealthComp->Health = pHealthComp->DefaultHealth;
	}
}

void AWeaponAI::Disable()
{
	SetActorHiddenInGame(true);

	SetActorEnableCollision(false);

	SetActorTickEnabled(false);
}

