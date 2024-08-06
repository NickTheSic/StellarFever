// Copyright Quarantine Games 2020


#include "MedicBay.h"
#include "../Player/BasePlayer.h"
#include "Engine/Engine.h"
#include "Math/UnrealMathUtility.h"
#include "TimerManager.h"
#include "Components/WidgetComponent.h"
#include "../Components/HealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "../Tutorial/TutorialManager2.h"
#include "EngineUtils.h"
#include "../StellarFeverGameState.h"
#include "Kismet/GameplayStatics.h"
#include "../Miscellaneous/ShipState.h"

// Sets default values
AMedicBay::AMedicBay()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	pMedicMesh = CreateDefaultSubobject<UStaticMeshComponent>("medic Collider");
	pMedicMesh->SetCollisionProfileName("BlockAll");
	RootComponent = pMedicMesh;

	ReserveComp = CreateDefaultSubobject<UWidgetComponent>("SupplyReserve");
	ReserveComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	SetReplicates(true);
	//SetReplicateMovement(true);

	bTutorial = false;


	Tags.Add("Medic Bay");
}

void AMedicBay::StartHeal(ABasePlayer* player)
{
	playerHealing = player;
	GetWorldTimerManager().SetTimer(HealDelay, this, &AMedicBay::Heal, HealingSpeed, true);

	//if in tutorial
	if (bTutorial)
	{
		//get tutorial manager
		for (TActorIterator<ATutorialManager2> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			//update MedBay bool
			if (ActorItr)
			{
				ActorItr->UpdateMedBay();
			}
		}
	}
}

void AMedicBay::Heal()
{
	if (playerHealing && playerHealing->GetHealthComponent()->GetHealthPercentage() != 1.0f && HealAvailable != 0)
	{
		playerHealing->RestoreHealth(HealAmount);
		HealAvailable -= HealAmount;
	}
}

void AMedicBay::StopHealing()
{
	GetWorldTimerManager().ClearTimer(HealDelay);
	playerHealing = nullptr;

	/*ActivateAutoRefill()*/
}

void AMedicBay::RefillingAvailableHealth()
{
	HealAvailable += HealingRate;
	//should clamp at max 600
	HealAvailable = FMath::Clamp(HealAvailable, 0, iDefaultHealthAvailable);

	//softcap is the lowest health reserve can go
	if (softRefillCap != 0 && HealAvailable >= softRefillCap)
	{
		softRefillCap = 0;
		GetWorldTimerManager().ClearTimer(HealRefill);
	}
	if (HealAvailable == iDefaultHealthAvailable)
		GetWorldTimerManager().ClearTimer(HealRefill);

}

void AMedicBay::StartRefillingMedBAy()
{
	//if healing has been used start the refilling timer
	if (HealAvailable < iDefaultHealthAvailable)
		GetWorldTimerManager().SetTimer(HealRefill, this, &AMedicBay::RefillingAvailableHealth, RefillSpeed, true);
}

void AMedicBay::PartialRefill(int amount)
{
	int temp = HealAvailable + amount;
	HealAvailable = FMath::Clamp(temp, softRefillCap, iDefaultHealthAvailable);
}

void AMedicBay::PartialRefillPercentage(float amount)
{
	if (amount < 1)
	{
		int temp = HealAvailable + iDefaultHealthAvailable * amount;
		HealAvailable = FMath::Clamp(temp, 0, iDefaultHealthAvailable);
	}
}

void AMedicBay::ActivateAutoRefill()
{
	if (HealAvailable < 600)
		GetWorldTimerManager().SetTimer(HealRefill, this, &AMedicBay::RefillingAvailableHealth, RefillSpeed, true);
}

void AMedicBay::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority)
	{
		AStellarFeverGameState* gameState = Cast<AStellarFeverGameState>(UGameplayStatics::GetGameState(GetWorld()));
		AShipState* shipstate = gameState->GetShipState();
		shipstate->SetMedReserves(this);
	}
}

void AMedicBay::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMedicBay, HealAvailable);

}