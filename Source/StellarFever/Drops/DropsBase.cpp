// Copyright Quarantine Games 2020


#include "DropsBase.h"
#include "../Player/BasePlayer.h"
#include "../Components/HealthComponent.h"
#include "Components/BoxComponent.h"
#include "Animation/AnimInstance.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "..\Components/InventoryComponent.h"
#include "..\Weapons/GunBase.h"
#include "..\Weapons/WeaponBase.h"
#include "Net/UnrealNetwork.h"
#include "Components/AudioComponent.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
ADropsBase::ADropsBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	pItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Item mesh");
	pItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	pItemMesh->SetCollisionProfileName("NoCollision");
	RootComponent = pItemMesh;

	pAudioComp = CreateDefaultSubobject<UAudioComponent>("sound effect");
	pAudioComp->SetupAttachment(RootComponent);


	pItemCollider = CreateDefaultSubobject<UBoxComponent>("Item Collider");
	pItemCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	pItemCollider->SetCollisionProfileName("OverlapAll");
	pItemCollider->SetupAttachment(RootComponent);

	SetReplicates(true);
	SetReplicateMovement(true);

	pParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>("particlesystem");
	pParticleSystem->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ADropsBase::BeginPlay()
{
	Super::BeginPlay();

	FVector location = FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z * 2.0f);
	//pParticleSystem = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Pparticle, location, FRotator::ZeroRotator, true);
	pParticleSystem->ActivateSystem();
	GetWorldTimerManager().SetTimer(FireTimer, this, &ADropsBase::DestroySelf, DespawnTimer);

}

// Called every frame
void ADropsBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADropsBase::DespawnSelf_Implementation()
{
	//if (pParticleSystem)
	pParticleSystem->DeactivateSystem();
	pParticleSystem->DestroyComponent();
	pAudioComp->SetSound(pEffect);
	pAudioComp->Play();
	pItemMesh->SetVisibility(false);
	float temp = pEffect->GetDuration();
	//Destroy();
	GetWorldTimerManager().SetTimer(FireTimer, this, &ADropsBase::DestroySelf, temp);
}

void ADropsBase::DestroySelf()
{
	Destroy();
}

//void ADropsBase::PickupSound()
//{
//	pAudioComp->SetSound(pEffect);
//	pAudioComp->Play();
//}

void ADropsBase::PickupSound_Implementation()
{
	pAudioComp->SetSound(pEffect);
	pAudioComp->Play();
}

void ADropsBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADropsBase, pItemMesh);
	DOREPLIFETIME(ADropsBase, pParticleSystem);
}