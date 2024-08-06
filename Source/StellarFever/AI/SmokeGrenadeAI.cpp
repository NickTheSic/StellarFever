// Copyright Quarantine Games 2020


#include "SmokeGrenadeAI.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "..\Player\BasePlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Math/TransformNonVectorized.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"

// Sets default values
ASmokeGrenadeAI::ASmokeGrenadeAI()
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
	pProjectileMovement->InitialSpeed = 1670.0f;
	pProjectileMovement->MaxSpeed = 1670.0f;
	pProjectileMovement->bShouldBounce = true;

	//NetUpdateFrequency = 8; //FRICKIN IMPORTANT!

	//SetReplicates(true);
	//SetReplicateMovement(true);
}

void ASmokeGrenadeAI::Explode()
{
	MulticastSelfDestruct();

	Destroy();
}

void ASmokeGrenadeAI::DestroyGrenade()
{
	pNiagaraEffect->DestroyComponent();
	pParticleSystem->DestroyComponent();
	pNiagaraEffect->Activate(false);
	pParticleSystem->Activate(false);
	pParticleSystem->DeactivateSystem();
	pNiagaraEffect->DestroyInstance();
	pNiagaraEffect->Deactivate();
	pNiagaraEffect->DeactivateImmediate();
	Destroy();

}

// Called when the game starts or when spawned
void ASmokeGrenadeAI::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorldTimerManager().SetTimer(TimerHandle_Explode, this, &ASmokeGrenadeAI::Explode, 1.f, false);

	FTimerHandle TimerHandle_DestroyGrenade;
	GetWorldTimerManager().SetTimer(TimerHandle_DestroyGrenade, this, &ASmokeGrenadeAI::DestroyGrenade, 3.2f, false);
}

// Called every frame
void ASmokeGrenadeAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASmokeGrenadeAI::MulticastSelfDestruct_Implementation()
{
	FRotator Orientation;
	FTransform transform = FTransform(Orientation, GetActorLocation(), FVector(5.f, 5.f, 5.f));
	pParticleSystem = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), pExplosionEffect, transform);
	UGameplayStatics::PlaySoundAtLocation(this, pExplodeSound, GetActorLocation());

	pNiagaraEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, pExplosionNiagara, GetActorLocation(), FRotator::ZeroRotator, FVector(6.f, 6.f, .6f));
}

bool ASmokeGrenadeAI::MulticastSelfDestruct_Validate()
{
	return true;
}