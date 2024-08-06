// Copyright Quarantine Games 2020


#include "TurretLaserProjectile.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "..\..\Player/BasePlayer.h"
#include "..\..\Interfaces/RaycastNotifyable.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ATurretLaserProjectile::ATurretLaserProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("CapsuleComp"));
	CollisionComp->BodyInstance.SetCollisionProfileName("BlockAll");
	CollisionComp->OnComponentHit.AddDynamic(this, &ATurretLaserProjectile::OnHit);

	//Players cant walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	RootComponent = CollisionComp;

	MeshStatic = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	MeshStatic->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshStatic->SetupAttachment(RootComponent);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.0f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	InitialLifeSpan = 3.0f;

	bAlwaysRelevant = true;
	SetReplicates(true);
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void ATurretLaserProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATurretLaserProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATurretLaserProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

	ABasePlayer* basePlayer = Cast<ABasePlayer>(OtherActor);
	if (basePlayer)
	{
		//basePlayer->MakeNoise(0.75f, nullptr, GetActorLocation()); //gets noise from weapon
		UGameplayStatics::ApplyDamage(basePlayer, fBaseDamage, GetInstigatorController(), this, DamageType);
		Destroy();
	}

	//IRaycastNotifyable* rayNotifyable = Cast<IRaycastNotifyable>(Hit.GetActor()); //Nicks code
	//if (rayNotifyable)
	//{
	//	rayNotifyable->NotifyHit(GetOwner(), &Hit);
	//}

	//MulticastFireExtras(TracerEndPoint);

}

