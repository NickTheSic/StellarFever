// Copyright Quarantine Games 2020


#include "AmmoBay.h"
#include "../Weapons/GunBase.h"
#include "../Weapons/GrenadeBase.h"
#include "TimerManager.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "../Tutorial/TutorialManager2.h"
#include "EngineUtils.h"
#include "../StellarFeverGameState.h"
#include "Kismet/GameplayStatics.h"
#include "../Miscellaneous/ShipState.h"

// Sets default values
AAmmoBay::AAmmoBay()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	pAmmoMesh = CreateDefaultSubobject<UStaticMeshComponent>("medic Collider");
	pAmmoMesh->SetCollisionProfileName("BlockAll");
	RootComponent = pAmmoMesh;

	MaxedRefills = 10;
	RefillsAllowed = MaxedRefills;

	ReserveComp = CreateDefaultSubobject<UWidgetComponent>("SupplyReserve");
	ReserveComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	SetReplicates(true);
	//SetReplicateMovement(true);

	bTutorial = false;

	/*AStellarFeverGameState* gameState = Cast<AStellarFeverGameState>(UGameplayStatics::GetGameState(GetWorld()));
	AShipState* shipstate = gameState->GetShipState();
	shipstate->SetAmmoReserve(this);*/

	Tags.Add("Ammo Bay");
}

void AAmmoBay::refillAmmo(AGunBase* gun)
{

	if (gun && RefillsAllowed != 0 && gun->GetReserveAmmo() < gun->GetMaxReserveAmmo())
	{
		gun->AmmoRefill(gun->GetMaxReserveAmmo());
		//gun->AmmoRefill(usbulletamount);
		RefillsAllowed--;
	}

	//AGunBase* weapon = Cast<AGunBase>(player->GetInventoryComponent()->GetGun());
	//float newAmmo = bullets->GetMaxReserveAmmo() / 4;
	//bullets->AmmoRefill(newAmmo);

	//if in tutorial
	if (bTutorial)
	{
		//get tutorial manager
		for (TActorIterator<ATutorialManager2> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			//update AmmoBay bool
			if (ActorItr)
			{
				ActorItr->UpdateAmmoBay();
			}
		}
	}
}

void AAmmoBay::StartRefillResource()
{
	GetWorldTimerManager().SetTimer(RefillDelay, this, &AAmmoBay::IncreaseRefillAmount, AmmoRefillRate, true);
}

void AAmmoBay::StopRefillResource()
{
	GetWorldTimerManager().ClearTimer(RefillDelay);

}

void AAmmoBay::IncreaseRefillAmount()
{
	if (RefillsAllowed < MaxedRefills)
		RefillsAllowed++;
}

void AAmmoBay::PartialRefill(int amount)
{
	int temp = RefillsAllowed + amount;

	RefillsAllowed = FMath::Clamp(temp, 0, MaxedRefills);
}


void AAmmoBay::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority)
	{
		AStellarFeverGameState* gameState = Cast<AStellarFeverGameState>(UGameplayStatics::GetGameState(GetWorld()));
		AShipState* shipstate = gameState->GetShipState();
		shipstate->SetAmmoReserve(this);
	}
}

void AAmmoBay::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAmmoBay, RefillsAllowed);
}