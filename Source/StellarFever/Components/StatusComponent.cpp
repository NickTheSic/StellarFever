// Copyright Quarantine Games 2020
/* Document initial creation by Nick Lemmon*/


#include "StatusComponent.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

#undef print
#if WITH_EDITOR
#define print(msg) GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Blue, msg);
#else
#define print(msg)
#endif

// Sets default values for this component's properties
UStatusComponent::UStatusComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	bCanRefill = false;
}


// Called every frame
void UStatusComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// if the tick is full then I don't want to tick this component anymore
	if (IsFull())
	{
		PrimaryComponentTick.bCanEverTick = false;
		return;
	}

	//If we are ticking then we are probably not full health
	if (bCanRefill)
	{
		GEngine->AddOnScreenDebugMessage(0, 5, FColor::Purple, "Status Component is Refilling");
		AddToValue(DeltaTime * fRefillRate);
	}

}

void UStatusComponent::SetValue(float val)
{
	fCurrentValue = FMath::Clamp<float> (val, 0 , fMaxValue);
}

void UStatusComponent::SubtractFromValue(float sub)
{
	fCurrentValue -= sub;

	if (fCurrentValue < 0.0f)
		fCurrentValue = 0.0f;

	DeplenishStartTimer();
}

void UStatusComponent::AddToValue(float add)
{
	fCurrentValue += add;

	if (fCurrentValue > fMaxValue)
		fCurrentValue = fMaxValue;

	if(bCanRefill == false)
		SetCanRefillTrue();

	//Check the world isn't null and check if the timer is active before we clear it
	if(GetWorld() && GetWorld()->GetTimerManager().IsTimerActive(Timer_RefillWait))
		GetWorld()->GetTimerManager().ClearTimer(Timer_RefillWait);
}

void UStatusComponent::SubtractFromMaxValue(float sub)
{
	//Subtract from the max value
	fMaxValue -= sub;

	//Max sure it doesn't go under 0
	if (fMaxValue < 0.0f)
		fMaxValue = 0.0f;

	//Keep the current value at max
	if (fCurrentValue > fMaxValue)
		fCurrentValue = fMaxValue;

}

void UStatusComponent::SetCanRefillTrue()
{
	bCanRefill = true; 
	PrimaryComponentTick.bCanEverTick = true;
}

void UStatusComponent::DeplenishStartTimer()
{
	bCanRefill = false;
	GetWorld()->GetTimerManager().ClearTimer(Timer_RefillWait);
	GetWorld()->GetTimerManager().SetTimer(Timer_RefillWait, this, &UStatusComponent::SetCanRefillTrue, fRefillWait, false);
}

#undef print

void UStatusComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UStatusComponent, fCurrentValue);
	DOREPLIFETIME(UStatusComponent, fMaxValue);

	DOREPLIFETIME(UStatusComponent, fRefillRate);
	DOREPLIFETIME(UStatusComponent, fRefillWait);

}
