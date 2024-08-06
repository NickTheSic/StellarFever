// Copyright Quarantine Games 2020
/* Document initial creation by Nick Lemmon*/

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatusComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STELLARFEVER_API UStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStatusComponent();

protected:

	/*The max value that this component can hold*/
	UPROPERTY(VisibleAnywhere, Replicated)
		float fMaxValue     = 100.0f;

	/*The current value that his component is at*/
	UPROPERTY(VisibleAnywhere, Replicated)
		float fCurrentValue = 100.0f;

	/*How quickly the component refills mulitplied by Delta Time */
	UPROPERTY(VisibleAnywhere, Replicated)
		float fRefillRate   =  40.0f;

	/*How long it takes before the component refills after the last time the current value was drained*/
	UPROPERTY(EditAnywhere,    Replicated)
		float fRefillWait   =   2.0f; 

	/*Timer handle to the to set and stop the timer for waiting to refill*/
	FTimerHandle Timer_RefillWait;

	/*A bool for tick that refills the status component*/
	bool bCanRefill = true;

	/*This function sets bCanRefill to true.  It is the function that gets called at the end of the timer*/
	UFUNCTION()
		void SetCanRefillTrue();

	/*Clears and Restarts the Timer.  Also sets bCanRefill to false*/
	void DeplenishStartTimer();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/*Blueprint callable function that returns the current value of our fCurrentValue*/
	UFUNCTION(BlueprintPure)
		float GetValue() { return fCurrentValue; }

	/*Sets the fCurrentValue to a specific value*/
	void SetValue(float val);

	//Set refill rate and clamp it
	void SetRefillRate(float val) { fRefillRate = FMath::Clamp<float>(val, 0.0f, fMaxValue); }
	//Set the refill wait time and clamp it
	void SetRefillWait(float val) { fRefillWait = FMath::Clamp<float>(val, 0.0f, fMaxValue); }

	/*A wrapper for subtracting from the fCurrentValue*/
	void SubtractFromValue(float sub);

	/*A wrapper for adding to the fCurrentValue*/
	void AddToValue(float add);

	/*A wrapper for subtracting from the max value*/
	void SubtractFromMaxValue(float sub);

	UFUNCTION(BlueprintPure)
	FORCEINLINE bool IsEmpty()        { return fCurrentValue <= 0.0f; }
	UFUNCTION(BlueprintPure)
	FORCEINLINE bool IsFull()         { return fCurrentValue >= fMaxValue; }
	UFUNCTION(BlueprintPure)
	FORCEINLINE float GetPercentage() { return fCurrentValue / fMaxValue; }


	void  SetMaxValue(float val) { fMaxValue = val; }
	UFUNCTION(BlueprintPure)
	FORCEINLINE float GetMaxValue()    { return fMaxValue; }
		
};
