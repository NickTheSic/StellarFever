// Copyright Quarantine Games 2020


#include "EnemyShield.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "../StellarFeverGameModeBase.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AEnemyShield::AEnemyShield()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	pShieldMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShieldMesh"));
	pShieldMesh->SetCollisionProfileName("BlockAll");
	RootComponent = pShieldCollider;

	pShieldCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("ShieldCollider"));
	pShieldCollider->SetCollisionProfileName("BlockAll");
	pShieldCollider->SetupAttachment(pShieldMesh);


	//fDefaultHealth = 80;
	//fShieldHealth = fDefaultHealth;

	fDefaultHealth = 60;
	fInitialHealth = fDefaultHealth;

	bDeactive = false;

	NetUpdateFrequency = 8; //FRICKIN IMPORTANT!

	SetReplicates(true);
	SetReplicateMovement(true);

	//this->Tags.Add("Enemy");
	this->Tags.Add("Shield");
}

void AEnemyShield::HurtShield(float DamageAmount)
{

	fShieldHealth -= DamageAmount;
	if (fShieldHealth <= 0)
	{
		bDeactive = true;

		Disable();
	}

}

// Called when the game starts or when spawned
void AEnemyShield::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AEnemyShield::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetOwner()->IsPendingKill())
	{
		Disable();
	}
}

void AEnemyShield::Enable()
{
	pShieldMesh->SetVisibility(true);
	SetActorHiddenInGame(false); //new

	SetActorEnableCollision(true);

	SetActorTickEnabled(true);

	AStellarFeverGameModeBase* pGameMode = Cast<AStellarFeverGameModeBase>(GetWorld()->GetAuthGameMode());
	if (pGameMode)
	{
		fDefaultHealth += 10.f * pGameMode->GetDifficulyValue();
		fShieldHealth = fDefaultHealth;
		
		fDefaultHealth = fInitialHealth + (20.f * pGameMode->GetDifficulyValue());
		fShieldHealth = fDefaultHealth;
	}
}

void AEnemyShield::Disable()
{
	pShieldMesh->SetVisibility(false);
	SetActorHiddenInGame(true); //new

	SetActorEnableCollision(false);

	SetActorTickEnabled(false);
}


void AEnemyShield::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(AEnemyShield, pShieldMesh);
	//DOREPLIFETIME(AEnemyShield, fDefaultHealth);
	//DOREPLIFETIME(AEnemyShield, fShieldHealth);
	//DOREPLIFETIME(AEnemyShield, pShieldCollider); 
	DOREPLIFETIME(AEnemyShield, bDeactive);
}