// Coded by Nick Lemmon

#pragma once

#include "CoreMinimal.h"
#include "BasePlayer.h"
#include "MedicPlayer.generated.h"

/**
 * This is the outline for the medic player
 * We inherit from the base player to have a single networked class
 * We override the UseSpecial() function and add the needed code to make it work here
 */
UCLASS()
class STELLARFEVER_API AMedicPlayer : public ABasePlayer
{
	GENERATED_BODY()
	
public:
	AMedicPlayer();

	virtual void Tick(float DeltaTime) override;
	bool UseSpecial() override; 

	UFUNCTION(BlueprintCallable)
		float GetMedicHealSpeed() { return fMedicHealSpeed; }

protected:

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
		float fMedicHealSpeed = 4.0f;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
		class UParticleSystemComponent* pParticleSystem = nullptr;

	virtual void TimerEndSpecial() override;


	UFUNCTION(NetMulticast, Unreliable)
		void Multicast_StartParticleEffect();
	UFUNCTION(Server, Unreliable)
		void Server_StartParticleEffect();
	UFUNCTION(NetMulticast, Unreliable)
		void Multicast_StopParticleEffect();
	UFUNCTION(Server, Unreliable)
		void Server_StopParticleEffect();
};
