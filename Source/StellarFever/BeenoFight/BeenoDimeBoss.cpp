// Copyright Quarantine Games 2021


#include "BeenoDimeBoss.h"
#include "Components/WidgetComponent.h"
#include "../Events/EnemyPool.h"
#include "Components/SkeletalMeshComponent.h"


// Sets default values
ABeenoDimeBoss::ABeenoDimeBoss()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	pMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh");
	SetRootComponent(pMesh);
}

// Called when the game starts or when spawned
void ABeenoDimeBoss::BeginPlay()
{
	Super::BeginPlay();
	fBossHealth = fMaxBossHealth;
	
}

// Called every frame
void ABeenoDimeBoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABeenoDimeBoss::DealDamage(AActor* actor, float Damage)
{
	if (!actor->ActorHasTag("Enemy"))
	{
		fBossHealth -= Damage;
	}
}
