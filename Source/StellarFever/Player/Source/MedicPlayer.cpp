// Coded by Nick


#include "../MedicPlayer.h"
#include "../../Components/HealthComponent.h"
#include "../../Components/PlayerSoundComponent.h"
#include "../../Components/InventoryComponent.h"
#include "../../Weapons/WeaponBase.h"
#include "Particles/ParticleSystemComponent.h"

AMedicPlayer::AMedicPlayer()
{
	//Setup class specific defaults for the Medic Player
	playerInformation.ePlayerClass = EPlayerClass::Medic;
	playerInformation.fActiveAbilityLength   = 15.0f;
	playerInformation.fActiveAbilityCooldown = 60.0f;

	Tags.Add("Medic");


	pParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Player Particle System"));
	pParticleSystem->SetupAttachment(GetRootComponent());

}

void AMedicPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetSpecialIsActive())
	{
		pHealthComponent->Heal(fMedicHealSpeed * DeltaTime);
	}
}

bool AMedicPlayer::UseSpecial()
{
	//We would play effects here
	Server_StartParticleEffect();

	return true;
}

void AMedicPlayer::TimerEndSpecial()
{
	Super::TimerEndSpecial();

	Server_StopParticleEffect();
}

void AMedicPlayer::Server_StartParticleEffect_Implementation()
{
	Multicast_StartParticleEffect();
}

void AMedicPlayer::Server_StopParticleEffect_Implementation()
{
	Multicast_StopParticleEffect();
}

void AMedicPlayer::Multicast_StartParticleEffect_Implementation()
{
	pParticleSystem->ActivateSystem();
}

void AMedicPlayer::Multicast_StopParticleEffect_Implementation()
{
	pParticleSystem->DeactivateSystem();
}
