// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PointLightComponent.h"
#include "EmergencyLightComponent.generated.h"

/**
 *\ 
 */
UCLASS()
class STELLARFEVER_API UEmergencyLightComponent : public UPointLightComponent
{
	GENERATED_BODY()
	
public:

	void HackedDoor();
	//turn red
	void FixedDoor();
	//normal color

	void doorsBroken();

	void doorsFixed();
	bool GetDoorStatus();
	void SetDoorStatus(bool stat);

public:
	bool bIsHacked = false;
	float fIntensity;

	UPROPERTY()
	FColor BrokenColor = FColor::Red;

	UPROPERTY()
	FColor fixedColor = FColor::Green;

	uint8 uiCounter;

};
