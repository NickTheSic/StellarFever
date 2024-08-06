// Copyright Quarantine Games 2020
/* Document initial creation by Nick Lemmon*/


#include "PlayerSoundComponent.h"

#undef SETUPSOUNDFUNCTION
#define NMC_SETUPSOUNDFUNCTION(soundname, val)                   \
void UPlayerSoundComponent::Play##soundname##_Implementation(){  \
	if (soundname != nullptr && FMath::RandRange(0,10) <= val) 								 \
	{                                                            \
		Stop();                                                  \
		SetSound(soundname); Play();							 \
	}                                                            \
};

NMC_SETUPSOUNDFUNCTION(AltFire               , 5);
NMC_SETUPSOUNDFUNCTION(AltFireRecharged      ,10);
NMC_SETUPSOUNDFUNCTION(AltMelee              , 5);
NMC_SETUPSOUNDFUNCTION(BeingHurt             , 5);
NMC_SETUPSOUNDFUNCTION(BoardingEnemyShip     , 9);
NMC_SETUPSOUNDFUNCTION(ClassAbility          ,10);
NMC_SETUPSOUNDFUNCTION(ClassAbilityRecharged ,10);
NMC_SETUPSOUNDFUNCTION(Dying                 ,10);
NMC_SETUPSOUNDFUNCTION(EnemiesBoarding       , 5);
NMC_SETUPSOUNDFUNCTION(EventInteractGeneral  , 5);
NMC_SETUPSOUNDFUNCTION(GrenadeRecharged      ,10);
NMC_SETUPSOUNDFUNCTION(GoingDown             ,10);
NMC_SETUPSOUNDFUNCTION(Gasping               , 4);
NMC_SETUPSOUNDFUNCTION(Idle                  , 3);
NMC_SETUPSOUNDFUNCTION(JumpingGrunt          , 4);
NMC_SETUPSOUNDFUNCTION(JumpSelection         ,10);
NMC_SETUPSOUNDFUNCTION(Killstreak            ,10);
NMC_SETUPSOUNDFUNCTION(LosingHealthEvent     ,10);
NMC_SETUPSOUNDFUNCTION(LosingOxygenEvent     ,10);
NMC_SETUPSOUNDFUNCTION(Respawning            ,10);
NMC_SETUPSOUNDFUNCTION(Reviving              ,10);
NMC_SETUPSOUNDFUNCTION(RunningGrunt          , 4);
NMC_SETUPSOUNDFUNCTION(ThankRevive           ,10);
NMC_SETUPSOUNDFUNCTION(WaitingRevive         ,10);

#undef NMC_SETUPSOUNDFUNCTION