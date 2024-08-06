// Copyright Quarantine Games 2020
/* Document initial creation by Nick Lemmon*/

#pragma once

#include "CoreMinimal.h"
#include "Components/AudioComponent.h"
#include "PlayerSoundComponent.generated.h"

/**
 * 
 */

#undef SETUPSOUNDFUNCTION
#define SETUPSOUNDFUNCTION(soundname)\
void Play##soundname(){\
	Stop();\
	if (##soundname != nullptr) {\
		SetSound(##soundname); Play();}\
};


UCLASS()
class STELLARFEVER_API UPlayerSoundComponent : public UAudioComponent
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere)
		USoundBase* AltFire               = nullptr;
	UPROPERTY(EditAnywhere)				  
		USoundBase* AltFireRecharged      = nullptr;
	UPROPERTY(EditAnywhere)	              
		USoundBase* AltMelee              = nullptr;
	//Begin Hurt can be taken down into Explode damage, Stab damage, gun damage and fire damage
	UPROPERTY(EditAnywhere)
		USoundBase* BeingHurt             = nullptr;				     
	UPROPERTY(EditAnywhere)			      
		USoundBase* BoardingEnemyShip     = nullptr;
	UPROPERTY(EditAnywhere)				  
		USoundBase* ClassAbility          = nullptr;
	UPROPERTY(EditAnywhere)
		USoundBase* ClassAbilityRecharged = nullptr;
	UPROPERTY(EditAnywhere)
		USoundBase* Dying                 = nullptr;
	UPROPERTY(EditAnywhere)				  
		USoundBase* EnemiesBoarding       = nullptr;
	UPROPERTY(EditAnywhere)				  
		USoundBase* EventInteractGeneral  = nullptr;
	UPROPERTY(EditAnywhere)				  
		USoundBase* Gasping               = nullptr;
	UPROPERTY(EditAnywhere)				  
		USoundBase* GoingDown             = nullptr;	
	UPROPERTY(EditAnywhere)
		USoundBase* GrenadeRecharged      = nullptr;
	UPROPERTY(EditAnywhere)		          
		USoundBase* Idle                  = nullptr;					         
	UPROPERTY(EditAnywhere)		          
		USoundBase* JumpingGrunt          = nullptr;					         
	UPROPERTY(EditAnywhere)		          
		USoundBase* JumpSelection         = nullptr;
	UPROPERTY(EditAnywhere)				  
		USoundBase* Killstreak            = nullptr;
	UPROPERTY(EditAnywhere)				  
		USoundBase* LosingHealthEvent     = nullptr;
	UPROPERTY(EditAnywhere)				  
		USoundBase* LosingOxygenEvent     = nullptr;
	UPROPERTY(EditAnywhere)				  
		USoundBase* Respawning            = nullptr;
	UPROPERTY(EditAnywhere)				  
		USoundBase* Reviving              = nullptr;
	UPROPERTY(EditAnywhere)				  
		USoundBase* RunningGrunt          = nullptr;
	UPROPERTY(EditAnywhere)				  
		USoundBase* ThankRevive           = nullptr;
	UPROPERTY(EditAnywhere)				  
		USoundBase* WaitingRevive         = nullptr;

public:

	UFUNCTION(NetMulticast, Reliable)
		void PlayAltFire();
	UFUNCTION(NetMulticast, Reliable)
		void PlayAltFireRecharged();
	UFUNCTION(NetMulticast, Reliable)
		void PlayAltMelee();
	UFUNCTION(NetMulticast, UnReliable)
		void PlayBeingHurt();
	UFUNCTION(NetMulticast, Reliable)
		void PlayBoardingEnemyShip();
	UFUNCTION(NetMulticast, Reliable)
		void PlayClassAbility();
	UFUNCTION(NetMulticast, Reliable)
		void PlayClassAbilityRecharged();
	UFUNCTION(NetMulticast, Reliable)
		void PlayDying();
	UFUNCTION(NetMulticast, Unreliable)
		void PlayEnemiesBoarding();
	UFUNCTION(NetMulticast, Reliable)
		void PlayEventInteractGeneral();
	UFUNCTION(NetMulticast, Reliable)
		void PlayGrenadeRecharged();
	UFUNCTION(NetMulticast, Reliable)
		void PlayGoingDown();
	UFUNCTION(NetMulticast, UnReliable)
		void PlayGasping();
	UFUNCTION(NetMulticast, Reliable)
		void PlayIdle();
	UFUNCTION(NetMulticast, UnReliable)
		void PlayJumpingGrunt();
	UFUNCTION(NetMulticast, Reliable)
		void PlayJumpSelection();
	UFUNCTION(NetMulticast, Reliable)
		void PlayKillstreak();
	UFUNCTION(NetMulticast, Reliable)
		void PlayLosingHealthEvent();
	UFUNCTION(NetMulticast, Reliable)
		void PlayLosingOxygenEvent();
	UFUNCTION(NetMulticast, Reliable)
		void PlayRespawning();
	UFUNCTION(NetMulticast, Reliable)
		void PlayReviving();
	UFUNCTION(NetMulticast, UnReliable)
		void PlayRunningGrunt();
	UFUNCTION(NetMulticast, Reliable)
		void PlayThankRevive();
	UFUNCTION(NetMulticast, Reliable)
		void PlayWaitingRevive();

	//SETUPSOUNDFUNCTION(AltFire);
	//SETUPSOUNDFUNCTION(AltFireRecharged);
	//SETUPSOUNDFUNCTION(AltMelee);
	//SETUPSOUNDFUNCTION(BeingHurt);
	//SETUPSOUNDFUNCTION(BoardingEnemyShip);
	//SETUPSOUNDFUNCTION(ClassAbility);
	//SETUPSOUNDFUNCTION(ClassAbilityRecharged);
	//SETUPSOUNDFUNCTION(Dying);
	//SETUPSOUNDFUNCTION(EnemiesBoarding);
	//SETUPSOUNDFUNCTION(EventInteractGeneral);
	//SETUPSOUNDFUNCTION(GrenadeRecharged);
	//SETUPSOUNDFUNCTION(GoingDown);
	//SETUPSOUNDFUNCTION(Gasping);
	//SETUPSOUNDFUNCTION(Idle);
	//SETUPSOUNDFUNCTION(JumpingGrunt);
	//SETUPSOUNDFUNCTION(JumpSelection);
	//SETUPSOUNDFUNCTION(Killstreak);
	//SETUPSOUNDFUNCTION(LosingHealthEvent);
	//SETUPSOUNDFUNCTION(LosingOxygenEvent);
	//SETUPSOUNDFUNCTION(Respawning);
	//SETUPSOUNDFUNCTION(Reviving);
	//SETUPSOUNDFUNCTION(RunningGrunt);
	//SETUPSOUNDFUNCTION(ThankRevive);
	//SETUPSOUNDFUNCTION(WaitingRevive);

};

#undef SETUPSOUNDFUNCTION
