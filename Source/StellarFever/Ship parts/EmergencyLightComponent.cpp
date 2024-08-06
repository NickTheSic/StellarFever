// Fill out your copyright notice in the Description page of Project Settings.

#include "EmergencyLightComponent.h"
#include "Math/Color.h"

void UEmergencyLightComponent::HackedDoor()
{	
	SetLightColor(BrokenColor);
	bIsHacked = true;
}

void UEmergencyLightComponent::FixedDoor()
{
	SetLightColor(fixedColor);
	bIsHacked = false;
}

void UEmergencyLightComponent::doorsBroken()
{
	SetIntensity(0);
}

void UEmergencyLightComponent::doorsFixed()
{
	SetIntensity(5000);
}

bool UEmergencyLightComponent::GetDoorStatus()
{
	return bIsHacked;
}

void UEmergencyLightComponent::SetDoorStatus(bool stat)
{
	bIsHacked = stat;
}
