// Fill out your copyright notice in the Description page of Project Settings.


#include "RaycastComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "../Interfaces/RaycastNotifyable.h"

// Sets default values for this component's properties
URaycastComponent::URaycastComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

#pragma region FCollisionObjectQueryParams
bool URaycastComponent::RaycastSingle(FHitResult& hit, FVector start, FRotator dir, float dist, FCollisionObjectQueryParams query, bool bNotifyRaycastHit)
{
	FVector end = start + dir.Vector() * dist;
	FCollisionQueryParams traceParams(FName(""), false, GetOwner());
	traceParams.bReturnPhysicalMaterial = true;
	bool result = GetWorld()->LineTraceSingleByObjectType(hit, start, end, query, traceParams);
	if (result && bNotifyRaycastHit && hit.GetActor())
	{
		//Check if hit actor is raycast notifyable, and if they are, notify them of the hit
		if (hit.GetActor()->Implements<URaycastNotifyable>())
			Cast<IRaycastNotifyable>(hit.GetActor())->NotifyHit(GetOwner(), &hit);
	}
	return result;
}

bool URaycastComponent::RaycastMulti(TArray<FHitResult>& hits, FVector start, FRotator dir, float dist, FCollisionObjectQueryParams query, bool bNotifyRaycastHit)
{
	FVector end = start + dir.Vector() * dist;
	FCollisionQueryParams traceParams(FName(""), false, GetOwner());
	traceParams.bReturnPhysicalMaterial = true;
	bool result = GetWorld()->LineTraceMultiByObjectType(hits, start, end, query, traceParams);
	if (result && bNotifyRaycastHit)
	{
		//Check if hit actors are raycast notifyable, and if they are, notify them of the hit
		for (const FHitResult& hit : hits)
		{
			if (hit.GetActor() && hit.GetActor()->Implements<URaycastNotifyable>())
				Cast<IRaycastNotifyable>(hit.GetActor())->NotifyHit(GetOwner(), &hit);
		}
	}
	return result;
}

bool URaycastComponent::RaycastSingle(FHitResult& hit, FVector start, FVector end, FCollisionObjectQueryParams query, bool bNotifyRaycastHit)
{
	FCollisionQueryParams traceParams(FName(""), false, GetOwner());
	traceParams.bReturnPhysicalMaterial = true;
	bool result = GetWorld()->LineTraceSingleByObjectType(hit, start, end, query, traceParams);
	if (result && bNotifyRaycastHit && hit.GetActor())
	{
		//Check if hit actor is raycast notifyable, and if they are, notify them of the hit
		if (hit.GetActor()->Implements<URaycastNotifyable>())
			Cast<IRaycastNotifyable>(hit.GetActor())->NotifyHit(GetOwner(), &hit);
	}
	return result;
}

bool URaycastComponent::RaycastMulti(TArray<FHitResult>& hits, FVector start, FVector end, FCollisionObjectQueryParams query, bool bNotifyRaycastHit)
{
	FCollisionQueryParams traceParams(FName(""), false, GetOwner());
	traceParams.bReturnPhysicalMaterial = true;
	bool result = GetWorld()->LineTraceMultiByObjectType(hits, start, end, query, traceParams);
	if (result && bNotifyRaycastHit)
	{
		//Check if hit actors are raycast notifyable, and if they are, notify them of the hit
		for (const FHitResult& hit : hits)
		{
			if (hit.GetActor() && hit.GetActor()->Implements<URaycastNotifyable>())
				Cast<IRaycastNotifyable>(hit.GetActor())->NotifyHit(GetOwner(), &hit);
		}
	}
	return result;
}

bool URaycastComponent::RaycastSingleFromPlayer(FHitResult& hit, float dist, FCollisionObjectQueryParams query, bool bNotifyRaycastHit)
{
	ACharacter* character = Cast<ACharacter>(GetOwner());
	if (character != nullptr)
	{
		//This component is attached to a character, so this only works on characters.
		AController* controller = character->GetController<AController>();
		if (controller != nullptr)
		{
			//Player has controller
			FVector start, end;
			FRotator rot;
			controller->GetPlayerViewPoint(start, rot);
			return RaycastSingle(hit, start, rot, dist, query, bNotifyRaycastHit);
		}
		else return false; //No controller, or invalid
	}
	else return false; //Was not a character
}

bool URaycastComponent::RaycastMultiFromPlayer(TArray<FHitResult>& hits, float dist, FCollisionObjectQueryParams query, bool bNotifyRaycastHit)
{
	ACharacter* character = Cast<ACharacter>(GetOwner());
	if (character != nullptr)
	{
		//This component is attached to a character, so this only works on characters.
		AController* controller = character->GetController<AController>();
		if (controller != nullptr)
		{
			//Player has controller
			FVector start, end;
			FRotator rot;
			controller->GetPlayerViewPoint(start, rot);
			return RaycastMulti(hits, start, rot, dist, query, bNotifyRaycastHit);
		}
		else return false; //No controller, or invalid
	}
	else return false; //Was not a character
}
#pragma endregion
#pragma region Channels
bool URaycastComponent::RaycastSingle(FHitResult& hit, FVector start, FRotator dir, float dist, ECollisionChannel channel, bool bNotifyRaycastHit)
{
	FVector end = start + dir.Vector() * dist;
	FCollisionQueryParams traceParams(FName(""), false, GetOwner());
	traceParams.bReturnPhysicalMaterial = true;
	bool result = GetWorld()->LineTraceSingleByChannel(hit, start, end, channel, traceParams);
	if (result && bNotifyRaycastHit && hit.GetActor())
	{
		//Check if hit actor is raycast notifyable, and if they are, notify them of the hit
		if (hit.GetActor()->Implements<URaycastNotifyable>())
			Cast<IRaycastNotifyable>(hit.GetActor())->NotifyHit(GetOwner(), &hit);
	}
	return result;
}

bool URaycastComponent::RaycastMulti(TArray<FHitResult>& hits, FVector start, FRotator dir, float dist, ECollisionChannel channel, bool bNotifyRaycastHit)
{
	FVector end = start + dir.Vector() * dist;
	FCollisionQueryParams traceParams(FName(""), false, GetOwner());
	traceParams.bReturnPhysicalMaterial = true;
	bool result = GetWorld()->LineTraceMultiByChannel(hits, start, end, channel, traceParams);
	if (result && bNotifyRaycastHit)
	{
		//Check if hit actors are raycast notifyable, and if they are, notify them of the hit
		for (const FHitResult& hit : hits)
		{
			if (hit.GetActor() && hit.GetActor()->Implements<URaycastNotifyable>())
				Cast<IRaycastNotifyable>(hit.GetActor())->NotifyHit(GetOwner(), &hit);
		}
	}
	return result;
}

bool URaycastComponent::RaycastSingle(FHitResult& hit, FVector start, FVector end, ECollisionChannel channel, bool bNotifyRaycastHit)
{
	FCollisionQueryParams traceParams(FName(""), false, GetOwner());
	traceParams.bReturnPhysicalMaterial = true;
	bool result = GetWorld()->LineTraceSingleByChannel(hit, start, end, channel, traceParams);
	if (result && bNotifyRaycastHit && hit.GetActor())
	{
		//Check if hit actor is raycast notifyable, and if they are, notify them of the hit
		if (hit.GetActor()->Implements<URaycastNotifyable>())
			Cast<IRaycastNotifyable>(hit.GetActor())->NotifyHit(GetOwner(), &hit);
	}
	return result;
}

bool URaycastComponent::RaycastMulti(TArray<FHitResult>& hits, FVector start, FVector end, ECollisionChannel channel, bool bNotifyRaycastHit)
{
	FCollisionQueryParams traceParams(FName(""), false, GetOwner());
	traceParams.bReturnPhysicalMaterial = true;
	bool result = GetWorld()->LineTraceMultiByChannel(hits, start, end, channel, traceParams);
	if (result && bNotifyRaycastHit)
	{
		//Check if hit actors are raycast notifyable, and if they are, notify them of the hit
		for (const FHitResult& hit : hits)
		{
			if (hit.GetActor() && hit.GetActor()->Implements<URaycastNotifyable>())
				Cast<IRaycastNotifyable>(hit.GetActor())->NotifyHit(GetOwner(), &hit);
		}
	}
	return result;
}

bool URaycastComponent::RaycastSingleFromPlayer(FHitResult& hit, float dist, ECollisionChannel channel, bool bNotifyRaycastHit)
{
	ACharacter* character = Cast<ACharacter>(GetOwner());
	if (character != nullptr)
	{
		//This component is attached to a character, so this only works on characters.
		AController* controller = character->GetController<AController>();
		if (controller != nullptr)
		{
			//Player has controller
			FVector start, end;
			FRotator rot;
			controller->GetPlayerViewPoint(start, rot);
			return RaycastSingle(hit, start, rot, dist, channel, bNotifyRaycastHit);
		}
		else return false; //No controller, or invalid
	}
	else return false; //Was not a character
}

bool URaycastComponent::RaycastMultiFromPlayer(TArray<FHitResult>& hits, float dist, ECollisionChannel channel, bool bNotifyRaycastHit)
{
	ACharacter* character = Cast<ACharacter>(GetOwner());
	if (character != nullptr)
	{
		//This component is attached to a character, so this only works on characters.
		AController* controller = character->GetController<AController>();
		if (controller != nullptr)
		{
			//Player has controller
			FVector start, end;
			FRotator rot;
			controller->GetPlayerViewPoint(start, rot);
			return RaycastMulti(hits, start, rot, dist, channel, bNotifyRaycastHit);
		}
		else return false; //No controller, or invalid
	}
	else return false; //Was not a character
}
#pragma endregion
#pragma region ProfileName
bool URaycastComponent::RaycastSingle(FHitResult& hit, FVector start, FRotator dir, float dist, FName name, bool bNotifyRaycastHit)
{
	FVector end = start + dir.Vector() * dist;
	FCollisionQueryParams traceParams(FName(""), false, GetOwner());
	traceParams.bReturnPhysicalMaterial = true;
	bool result = GetWorld()->LineTraceSingleByProfile(hit, start, end, name, traceParams);
	if (result && bNotifyRaycastHit && hit.GetActor())
	{
		//Check if hit actor is raycast notifyable, and if they are, notify them of the hit
		if (hit.GetActor()->Implements<URaycastNotifyable>())
			Cast<IRaycastNotifyable>(hit.GetActor())->NotifyHit(GetOwner(), &hit);
	}
	return result;
}

bool URaycastComponent::RaycastMulti(TArray<FHitResult>& hits, FVector start, FRotator dir, float dist, FName name, bool bNotifyRaycastHit)
{
	FVector end = start + dir.Vector() * dist;
	FCollisionQueryParams traceParams(FName(""), false, GetOwner());
	traceParams.bReturnPhysicalMaterial = true;
	bool result = GetWorld()->LineTraceMultiByProfile(hits, start, end, name, traceParams);
	if (result && bNotifyRaycastHit)
	{
		//Check if hit actors are raycast notifyable, and if they are, notify them of the hit
		for (const FHitResult& hit : hits)
		{
			if (hit.GetActor() && hit.GetActor()->Implements<URaycastNotifyable>())
				Cast<IRaycastNotifyable>(hit.GetActor())->NotifyHit(GetOwner(), &hit);
		}
	}
	return result;
}

bool URaycastComponent::RaycastSingle(FHitResult& hit, FVector start, FVector end, FName name, bool bNotifyRaycastHit)
{
	FCollisionQueryParams traceParams(FName(""), false, GetOwner());
	traceParams.bReturnPhysicalMaterial = true;
	bool result = GetWorld()->LineTraceSingleByProfile(hit, start, end, name, traceParams);
	if (result && bNotifyRaycastHit && hit.GetActor())
	{
		//Check if hit actor is raycast notifyable, and if they are, notify them of the hit
		if (hit.GetActor()->Implements<URaycastNotifyable>())
			Cast<IRaycastNotifyable>(hit.GetActor())->NotifyHit(GetOwner(), &hit);
	}
	return result;
}

bool URaycastComponent::RaycastMulti(TArray<FHitResult>& hits, FVector start, FVector end, FName name, bool bNotifyRaycastHit)
{
	FCollisionQueryParams traceParams(FName(""), false, GetOwner());
	traceParams.bReturnPhysicalMaterial = true;
	bool result = GetWorld()->LineTraceMultiByProfile(hits, start, end, name, traceParams);
	if (result && bNotifyRaycastHit)
	{
		//Check if hit actors are raycast notifyable, and if they are, notify them of the hit
		for (const FHitResult& hit : hits)
		{
			if (hit.GetActor() && hit.GetActor()->Implements<URaycastNotifyable>())
				Cast<IRaycastNotifyable>(hit.GetActor())->NotifyHit(GetOwner(), &hit);
		}
	}
	return result;
}

bool URaycastComponent::RaycastSingleFromPlayer(FHitResult& hit, float dist, FName name, bool bNotifyRaycastHit)
{
	ACharacter* character = Cast<ACharacter>(GetOwner());
	if (character != nullptr)
	{
		//This component is attached to a character, so this only works on characters.
		AController* controller = character->GetController<AController>();
		if (controller != nullptr)
		{
			//Player has controller
			FVector start, end;
			FRotator rot;
			controller->GetPlayerViewPoint(start, rot);
			return RaycastSingle(hit, start, rot, dist, name, bNotifyRaycastHit);
		}
		else return false; //No controller, or invalid
	}
	else return false; //Was not a character
}

bool URaycastComponent::RaycastMultiFromPlayer(TArray<FHitResult>& hits, float dist, FName name, bool bNotifyRaycastHit)
{
	ACharacter* character = Cast<ACharacter>(GetOwner());
	if (character != nullptr)
	{
		//This component is attached to a character, so this only works on characters.
		AController* controller = character->GetController<AController>();
		if (controller != nullptr)
		{
			//Player has controller
			FVector start, end;
			FRotator rot;
			controller->GetPlayerViewPoint(start, rot);
			return RaycastMulti(hits, start, rot, dist, name, bNotifyRaycastHit);
		}
		else return false; //No controller, or invalid
	}
	else return false; //Was not a character
}

#pragma endregion