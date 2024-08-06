// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
//#include "GameFramework/PlayerController.h"
#include "RaycastComponent.generated.h"

//Quick setup for use:
//1. Attach component to actor
//2. Call any of the functions
//E.g.:
//			FHitResult ray;
//			if (pRaycastComponent->RaycastSingleFromPlayer(ray, 150.f))
//			{
//				//Raycast succeeded
//			}


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STELLARFEVER_API URaycastComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URaycastComponent();

public:	
#pragma region Standard ECC Declarations
	/*	Returns true if raycast found something. Data about raycast is stored in hit.
		FHitResult& hit: The raycast result;
		FVector start: The start of the raycast;
		FRotator dir: The angle to raycast towards;
		float dist: The distance from the start of the raycast to the end of it;
		FCollisionObjectQueryParams query: Extra query data. Set to ECollisionChannel::ECC_PhysicsBody by default;
		bool bNotifyRaycastHit: Whether or not to notify any IRaycastNotifyable instances of a hit. Set to false by default;
		RETURNS bool: If the raycast hit anything, returns true.
	*/
	bool RaycastSingle(FHitResult& hit, FVector start, FRotator dir, float dist, FCollisionObjectQueryParams query = ECollisionChannel::ECC_PhysicsBody, bool bNotifyRaycastHit = false);
	/*	Returns true if raycast found something. Data about raycast is stored in hit.
		FHitResult& hit: The raycast result;
		FVector start: The start of the raycast;
		FVector end: The end of the raycast;
		FCollisionObjectQueryParams query: Extra query data. Set to ECollisionChannel::ECC_PhysicsBody by default;
		bool bNotifyRaycastHit: Whether or not to notify any IRaycastNotifyable instances of a hit. Set to false by default;
		RETURNS bool: If the raycast hit anything, returns true.
	*/
	bool RaycastSingle(FHitResult& hit, FVector start, FVector end, FCollisionObjectQueryParams query = ECollisionChannel::ECC_PhysicsBody, bool bNotifyRaycastHit = false);
	/*	Returns true if raycast found something. Data about raycast is stored in hits.
		TArray<FHitResult>& hit: All raycast results;
		FVector start: The start of the raycast;
		FRotator dir: The angle to raycast towards;
		float dist: The distance from the start of the raycast to the end of it;
		FCollisionObjectQueryParams query: Extra query data. Set to ECollisionChannel::ECC_PhysicsBody by default;
		bool bNotifyRaycastHit: Whether or not to notify any IRaycastNotifyable instances of a hit. Set to false by default;
		RETURNS bool: If the raycast hit anything, returns true.
	*/
	bool RaycastMulti(TArray<FHitResult>& hits, FVector start, FRotator dir, float dist, FCollisionObjectQueryParams query = ECollisionChannel::ECC_PhysicsBody, bool bNotifyRaycastHit = false);
	/*	Returns true if raycast found something. Data about raycast is stored in hits.
		TArray<FHitResult>& hit: All raycast results;
		FVector start: The start of the raycast;
		FVector end: The end of the raycast;
		FCollisionObjectQueryParams query: Extra query data. Set to ECollisionChannel::ECC_PhysicsBody by default;
		bool bNotifyRaycastHit: Whether or not to notify any IRaycastNotifyable instances of a hit. Set to false by default;
		RETURNS bool: If the raycast hit anything, returns true.
	*/
	bool RaycastMulti(TArray<FHitResult>& hits, FVector start, FVector end, FCollisionObjectQueryParams query = ECollisionChannel::ECC_PhysicsBody, bool bNotifyRaycastHit = false);
	/*	Performs a raycast starting at the player, towards where they are looking.
		Returns true if raycast found something. Data about raycast is stored in hit.
		Parent must be of type ACharacter, and must possess an APlayerController*
		FHitResult& hit: The raycast result;
		float dist: The distance from the start of the raycast to the end of it;
		FCollisionObjectQueryParams query: Extra query data. Set to ECollisionChannel::ECC_PhysicsBody by default;
		bool bNotifyRaycastHit: Whether or not to notify any IRaycastNotifyable instances of a hit. Set to false by default;
		RETURNS bool: If the raycast hit anything, returns true.
	*/
	bool RaycastSingleFromPlayer(FHitResult& hit, float dist, FCollisionObjectQueryParams query = ECollisionChannel::ECC_PhysicsBody, bool bNotifyRaycastHit = false);
	/*	Performs a raycast starting at the player, towards where they are looking.
		Returns true if raycast found something. Data about raycast is stored in hit.
		Parent must be of type ACharacter, and must possess an APlayerController*
		TArray<FHitResult>& hit: The raycast results;
		float dist: The distance from the start of the raycast to the end of it;
		FCollisionObjectQueryParams query: Extra query data. Set to ECollisionChannel::ECC_PhysicsBody by default;
		bool bNotifyRaycastHit: Whether or not to notify any IRaycastNotifyable instances of a hit. Set to false by default;
		RETURNS bool: If the raycast hit anything, returns true.
	*/
	bool RaycastMultiFromPlayer(TArray<FHitResult>& hit, float dist, FCollisionObjectQueryParams query = ECollisionChannel::ECC_PhysicsBody, bool bNotifyRaycastHit = false);
	
	/*	Returns true if raycast found something. Data about raycast is stored in hit.
		FHitResult& hit: The raycast result;
		FVector start: The start of the raycast;
		FRotator dir: The angle to raycast towards;
		float dist: The distance from the start of the raycast to the end of it;
		FName profileName: the raycast profile name to check;
		bool bNotifyRaycastHit: Whether or not to notify any IRaycastNotifyable instances of a hit. Set to false by default;
		RETURNS bool: If the raycast hit anything, returns true.
	*/
#pragma endregion
#pragma region Channel Declarations
	/*	Returns true if raycast found something. Data about raycast is stored in hit.
		FHitResult& hit: The raycast result;
		FVector start: The start of the raycast;
		FRotator dir: The angle to raycast towards;
		float dist: The distance from the start of the raycast to the end of it;
		ECollisionChannel channel: Extra query data. Set to ECollisionChannel::ECC_MAX by default;
		bool bNotifyRaycastHit: Whether or not to notify any IRaycastNotifyable instances of a hit. Set to false by default;
		RETURNS bool: If the raycast hit anything, returns true.
	*/
	bool RaycastSingle(FHitResult& hit, FVector start, FRotator dir, float dist, ECollisionChannel channel = ECollisionChannel::ECC_MAX, bool bNotifyRaycastHit = false);
	/*	Returns true if raycast found something. Data about raycast is stored in hit.
		FHitResult& hit: The raycast result;
		FVector start: The start of the raycast;
		FVector end: The end of the raycast;
		ECollisionChannel channel: Extra query data. Set to ECollisionChannel::ECC_MAX by default;
		bool bNotifyRaycastHit: Whether or not to notify any IRaycastNotifyable instances of a hit. Set to false by default;
		RETURNS bool: If the raycast hit anything, returns true.
	*/
	bool RaycastSingle(FHitResult& hit, FVector start, FVector end, ECollisionChannel channel = ECollisionChannel::ECC_MAX, bool bNotifyRaycastHit = false);
	/*	Returns true if raycast found something. Data about raycast is stored in hits.
		TArray<FHitResult>& hit: All raycast results;
		FVector start: The start of the raycast;
		FRotator dir: The angle to raycast towards;
		float dist: The distance from the start of the raycast to the end of it;
		ECollisionChannel channel: Extra query data. Set to ECollisionChannel::ECC_MAX by default;
		bool bNotifyRaycastHit: Whether or not to notify any IRaycastNotifyable instances of a hit. Set to false by default;
		RETURNS bool: If the raycast hit anything, returns true.
	*/
	bool RaycastMulti(TArray<FHitResult>& hits, FVector start, FRotator dir, float dist, ECollisionChannel channel = ECollisionChannel::ECC_MAX, bool bNotifyRaycastHit = false);
	/*	Returns true if raycast found something. Data about raycast is stored in hits.
		TArray<FHitResult>& hit: All raycast results;
		FVector start: The start of the raycast;
		FVector end: The end of the raycast;
		ECollisionChannel channel: Extra query data. Set to ECollisionChannel::ECC_MAX by default;
		bool bNotifyRaycastHit: Whether or not to notify any IRaycastNotifyable instances of a hit. Set to false by default;
		RETURNS bool: If the raycast hit anything, returns true.
	*/
	bool RaycastMulti(TArray<FHitResult>& hits, FVector start, FVector end, ECollisionChannel channel = ECollisionChannel::ECC_MAX, bool bNotifyRaycastHit = false);
	/*	Performs a raycast starting at the player, towards where they are looking.
		Returns true if raycast found something. Data about raycast is stored in hit.
		Parent must be of type ACharacter, and must possess an APlayerController*
		FHitResult& hit: The raycast result;
		float dist: The distance from the start of the raycast to the end of it;
		ECollisionChannel channel: Extra query data. Set to ECollisionChannel::ECC_MAX by default;
		bool bNotifyRaycastHit: Whether or not to notify any IRaycastNotifyable instances of a hit. Set to false by default;
		RETURNS bool: If the raycast hit anything, returns true.
	*/
	bool RaycastSingleFromPlayer(FHitResult& hit, float dist, ECollisionChannel channel = ECollisionChannel::ECC_MAX, bool bNotifyRaycastHit = false);
	/*	Performs a raycast starting at the player, towards where they are looking.
		Returns true if raycast found something. Data about raycast is stored in hit.
		Parent must be of type ACharacter, and must possess an APlayerController*
		TArray<FHitResult>& hit: The raycast results;
		float dist: The distance from the start of the raycast to the end of it;
		ECollisionChannel channel: Extra query data. Set to ECollisionChannel::ECC_MAX by default;
		bool bNotifyRaycastHit: Whether or not to notify any IRaycastNotifyable instances of a hit. Set to false by default;
		RETURNS bool: If the raycast hit anything, returns true.
	*/
	bool RaycastMultiFromPlayer(TArray<FHitResult>& hit, float dist, ECollisionChannel channel = ECollisionChannel::ECC_MAX, bool bNotifyRaycastHit = false);
#pragma endregion
#pragma region Standard ProfileName Declarations
	/*	Returns true if raycast found something. Data about raycast is stored in hit.
		FHitResult& hit: The raycast result;
		FVector start: The start of the raycast;
		FRotator dir: The angle to raycast towards;
		float dist: The distance from the start of the raycast to the end of it;
		FName profileName: the raycast profile name to check;
		bool bNotifyRaycastHit: Whether or not to notify any IRaycastNotifyable instances of a hit. Set to false by default;
		RETURNS bool: If the raycast hit anything, returns true.
	*/
	bool RaycastSingle(FHitResult& hit, FVector start, FRotator dir, float dist, FName profileName, bool bNotifyRaycastHit = false);
	/*	Returns true if raycast found something. Data about raycast is stored in hit.
		FHitResult& hit: The raycast result;
		FVector start: The start of the raycast;
		FVector end: The end of the raycast;
		FName profileName: the raycast profile name to check;
		bool bNotifyRaycastHit: Whether or not to notify any IRaycastNotifyable instances of a hit. Set to false by default;
		RETURNS bool: If the raycast hit anything, returns true.
	*/
	bool RaycastSingle(FHitResult& hit, FVector start, FVector end, FName profileName, bool bNotifyRaycastHit = false);
	/*	Returns true if raycast found something. Data about raycast is stored in hit.
		TArray<FHitResult>& hits: The raycast results;
		FVector start: The start of the raycast;
		FRotator dir: The angle to raycast towards;
		float dist: The distance from the start of the raycast to the end of it;
		FName profileName: the raycast profile name to check;
		bool bNotifyRaycastHit: Whether or not to notify any IRaycastNotifyable instances of a hit. Set to false by default;
		RETURNS bool: If the raycast hit anything, returns true.
	*/
	bool RaycastMulti(TArray<FHitResult>& hits, FVector start, FRotator dir, float dist, FName profileName, bool bNotifyRaycastHit = false);
	/*	Returns true if raycast found something. Data about raycast is stored in hit.
		TArray<FHitResult>& hits: The raycast results;
		FVector start: The start of the raycast;
		FVector end: The end of the raycast;
		FName profileName: the raycast profile name to check;
		bool bNotifyRaycastHit: Whether or not to notify any IRaycastNotifyable instances of a hit. Set to false by default;
		RETURNS bool: If the raycast hit anything, returns true.
	*/
	bool RaycastMulti(TArray<FHitResult>& hits, FVector start, FVector, FName profileName, bool bNotifyRaycastHit = false);
	/*	Performs a raycast starting at the player, towards where they are looking.
		Returns true if raycast found something. Data about raycast is stored in hit.
		Parent must be of type ACharacter, and must possess an APlayerController*
		TArray<FHitResult>& hit: The raycast results;
		float dist: The distance from the start of the raycast to the end of it;
		FName profileName: the raycast profile name to check;
		bool bNotifyRaycastHit: Whether or not to notify any IRaycastNotifyable instances of a hit. Set to false by default;
		RETURNS bool: If the raycast hit anything, returns true.
	*/
	bool RaycastSingleFromPlayer(FHitResult& hit, float dist, FName profileName, bool bNotifyRaycastHit = false);
	/*	Performs a raycast starting at the player, towards where they are looking.
		Returns true if raycast found something. Data about raycast is stored in hit.
		Parent must be of type ACharacter, and must possess an APlayerController*
		TArray<FHitResult>& hit: The raycast results;
		float dist: The distance from the start of the raycast to the end of it;
		FName profileName: the raycast profile name to check;
		bool bNotifyRaycastHit: Whether or not to notify any IRaycastNotifyable instances of a hit. Set to false by default;
		RETURNS bool: If the raycast hit anything, returns true.
	*/
	bool RaycastMultiFromPlayer(TArray<FHitResult>& hit, float dist, FName profileName, bool bNotifyRaycastHit = false);
	/*	Returns true if raycast found something. Data about raycast is stored in hit.
		Only returns true if hit actor type is the one provided.
		FHitResult& hit: The raycast result;
		FVector start: The start of the raycast;
		FRotator dir: The angle to raycast towards;
		float dist: The distance from the start of the raycast to the end of it;
		FCollisionObjectQueryParams query: Extra query data. Set to ECollisionChannel::ECC_PhysicsBody by default;
		bool bNotifyRaycastHit: Whether or not to notify any IRaycastNotifyable instances of a hit. Set to false by default;
		RETURNS bool: If the raycast hit anything, returns true.
	*/
#pragma endregion
#pragma region Template ECC Declarations
	template<class T>
	bool RaycastSingleT(FHitResult& hit, FVector start, FRotator dir, float dist, FCollisionObjectQueryParams query = ECollisionChannel::ECC_PhysicsBody, bool bNotifyRaycastHit = false)
	{
		TArray<FHitResult> hits;
		if (RaycastMultiT<T>(hits, start, dir, dist, query, bNotifyRaycastHit))
		{
			hit = hits[0];
			return true;
		}
		else return false;
	}
	/*	Returns true if raycast found something. Data about raycast is stored in hit.
		Only returns true if hit actor type is the one provided.
		FHitResult& hit: The raycast result;
		FVector start: The start of the raycast;
		FVector start: The end of the raycast;
		FCollisionObjectQueryParams query: Extra query data. Set to ECollisionChannel::ECC_PhysicsBody by default;
		bool bNotifyRaycastHit: Whether or not to notify any IRaycastNotifyable instances of a hit. Set to false by default;
		RETURNS bool: If the raycast hit anything, returns true.
	*/
	template<class T>
	bool RaycastSingleT(FHitResult& hit, FVector start, FVector end, FCollisionObjectQueryParams query = ECollisionChannel::ECC_PhysicsBody, bool bNotifyRaycastHit = false)
	{
		TArray<FHitResult> hits;
		if (RaycastMultiT<T>(hits, start, end, query, bNotifyRaycastHit))
		{
			hit = hits[0];
			return true;
		}
		else return false;
	}
	/*	Returns true if raycast found something. Data about raycast is stored in hits.
		Only returns true if hit actor type is the one provided.
		TArray<FHitResult>& hit: All raycast results;
		FVector start: The start of the raycast;
		FRotator dir: The angle to raycast towards;
		float dist: The distance from the start of the raycast to the end of it;
		FCollisionObjectQueryParams query: Extra query data. Set to ECollisionChannel::ECC_PhysicsBody by default;
		bool bNotifyRaycastHit: Whether or not to notify any IRaycastNotifyable instances of a hit. Set to false by default;
		RETURNS bool: If the raycast hit anything, returns true.
	*/
	template<class T>
	bool RaycastMultiT(TArray<FHitResult>& hits, FVector start, FRotator dir, float dist, FCollisionObjectQueryParams query = ECollisionChannel::ECC_PhysicsBody, bool bNotifyRaycastHit = false)
	{
		if (RaycastMulti(hits, start, dir, dist, query, bNotifyRaycastHit))
		{
			hits.RemoveAllSwap([](FHitResult& hit) -> bool
			{
				return Cast<T>(hit.GetActor()) == nullptr;
			});
			return hits.Num() > 0;
		}
		else return false;
	}
	/*	Returns true if raycast found something. Data about raycast is stored in hits.
		Only returns true if hit actor type is the one provided.
		TArray<FHitResult>& hit: All raycast results;
		FVector start: The start of the raycast;
		FVector end: The end of the raycast;
		FCollisionObjectQueryParams query: Extra query data. Set to ECollisionChannel::ECC_PhysicsBody by default;
		bool bNotifyRaycastHit: Whether or not to notify any IRaycastNotifyable instances of a hit. Set to false by default;
		RETURNS bool: If the raycast hit anything, returns true.
	*/
	template<class T>
	bool RaycastMultiT(TArray<FHitResult>& hits, FVector start, FVector end, FCollisionObjectQueryParams query = ECollisionChannel::ECC_PhysicsBody, bool bNotifyRaycastHit = false)
	{
		if (RaycastMulti(hits, start, end, query, bNotifyRaycastHit))
		{
			hits.RemoveAllSwap([](FHitResult& hit) -> bool
			{
				return Cast<T>(hit.GetActor()) == nullptr;
			});
			return hits.Num() > 0;
		}
		else return false;
	}
	/*	Performs a raycast starting at the player, towards where they are looking.
		Returns true if raycast found something. Data about raycast is stored in hit.
		Only returns true if hit actor type is the one provided.
		Parent must be of type ACharacter, and must possess an APlayerController*
		FHitResult& hit: The raycast result;
		float dist: The distance from the start of the raycast to the end of it;
		FName profileName: the raycast profile name to check;
		bool bNotifyRaycastHit: Whether or not to notify any IRaycastNotifyable instances of a hit. Set to false by default;
		RETURNS bool: If the raycast hit anything, returns true.
	*/
	template<class T>
	bool RaycastSingleFromPlayerT(FHitResult& hit, float dist, FCollisionObjectQueryParams query = ECollisionChannel::ECC_PhysicsBody, bool bNotifyRaycastHit = false)
	{
		TArray<FHitResult> hits;
		if (RaycastMultiFromPlayerT<T>(hits, dist, query, bNotifyRaycastHit))
		{
			hit = hits[0];
			return true;
		}
		else return false;
	}
	/*	Performs a raycast starting at the player, towards where they are looking.
		Returns true if raycast found something. Data about raycast is stored in hit.
		Only returns true if hit actor type is the one provided.
		Parent must be of type ACharacter, and must possess an APlayerController*
		TArray<FHitResult>& hit: The raycast results;
		float dist: The distance from the start of the raycast to the end of it;
		FName profileName: the raycast profile name to check;
		bool bNotifyRaycastHit: Whether or not to notify any IRaycastNotifyable instances of a hit. Set to false by default;
		RETURNS bool: If the raycast hit anything, returns true.
	*/
	template<class T>
	bool RaycastMultiFromPlayerT(TArray<FHitResult>& hits, float dist, FCollisionObjectQueryParams query = ECollisionChannel::ECC_PhysicsBody, bool bNotifyRaycastHit = false)
	{
		if (RaycastMultiFromPlayer(hits, dist, query, bNotifyRaycastHit))
		{
			hits.RemoveAllSwap([](FHitResult& hit) -> bool
			{
				return Cast<T>(hit.GetActor()) == nullptr;
			});
			return hits.Num() > 0;
		}
		else return false;
	}/*	Returns true if raycast found something. Data about raycast is stored in hit.
		Only returns true if hit actor type is the one provided.
		FHitResult& hit: The raycast result;
		FVector start: The start of the raycast;
		FVector start: The end of the raycast;
		FName profileName: the raycast profile name to check;
		bool bNotifyRaycastHit: Whether or not to notify any IRaycastNotifyable instances of a hit. Set to false by default;
		RETURNS bool: If the raycast hit anything, returns true.
	*/
#pragma endregion
#pragma region Template ProfileName Declarations
	template<class T>
	bool RaycastSingleT(FHitResult& hit, FVector start, FRotator dir, float dist, FName profileName, bool bNotifyRaycastHit = false)
	{
		TArray<FHitResult> hits;
		if (RaycastMultiT<T>(hits, start, dir, dist, profileName, bNotifyRaycastHit))
		{
			hit = hits[0];
			return true;
		}
		else return false;
	}
	/*	Returns true if raycast found something. Data about raycast is stored in hits.
		Only returns true if hit actor type is the one provided.
		TArray<FHitResult>& hit: All raycast results;
		FVector start: The start of the raycast;
		FRotator dir: The angle to raycast towards;
		float dist: The distance from the start of the raycast to the end of it;
		FName profileName: the raycast profile name to check;
		bool bNotifyRaycastHit: Whether or not to notify any IRaycastNotifyable instances of a hit. Set to false by default;
		RETURNS bool: If the raycast hit anything, returns true.
	*/
	template<class T>
	bool RaycastSingleT(FHitResult& hit, FVector start, FVector end, FName profileName, bool bNotifyRaycastHit = false)
	{
		TArray<FHitResult> hits;
		if (RaycastMultiT<T>(hits, start, end, profileName, bNotifyRaycastHit))
		{
			hit = hits[0];
			return true;
		}
		else return false;
	}
	/*	Returns true if raycast found something. Data about raycast is stored in hits.
		Only returns true if hit actor type is the one provided.
		TArray<FHitResult>& hit: All raycast results;
		FVector start: The start of the raycast;
		FRotator dir: The angle to raycast towards;
		float dist: The distance from the start of the raycast to the end of it;
		FName profileName: the raycast profile name to check;
		bool bNotifyRaycastHit: Whether or not to notify any IRaycastNotifyable instances of a hit. Set to false by default;
		RETURNS bool: If the raycast hit anything, returns true.
	*/
	template<class T>
	bool RaycastMultiT(TArray<FHitResult>& hits, FVector start, FRotator dir, float dist, FName profileName, bool bNotifyRaycastHit = false)
	{
		if (RaycastMulti(hits, start, dir, dist, profileName, bNotifyRaycastHit))
		{
			hits.RemoveAllSwap([](FHitResult& hit) -> bool
			{
				return Cast<T>(hit.GetActor()) == nullptr;
			});
			return hits.Num() > 0;
		}
		else return false;
	}
	/*	Returns true if raycast found something. Data about raycast is stored in hits.
		Only returns true if hit actor type is the one provided.
		TArray<FHitResult>& hit: All raycast results;
		FVector start: The start of the raycast;
		FVector end: The end of the raycast;
		FName profileName: the raycast profile name to check;
		bool bNotifyRaycastHit: Whether or not to notify any IRaycastNotifyable instances of a hit. Set to false by default;
		RETURNS bool: If the raycast hit anything, returns true.
	*/
	template<class T>
	bool RaycastMultiT(TArray<FHitResult>& hits, FVector start, FVector end, FName profileName, bool bNotifyRaycastHit = false)
	{
		if (RaycastMulti(hits, start, end, profileName, bNotifyRaycastHit))
		{
			hits.RemoveAllSwap([](FHitResult& hit) -> bool
			{
				return Cast<T>(hit.GetActor()) == nullptr;
			});
			return hits.Num() > 0;
		}
		else return false;
	}
	/*	Performs a raycast starting at the player, towards where they are looking.
		Returns true if raycast found something. Data about raycast is stored in hit.
		Only returns true if hit actor type is the one provided.
		Parent must be of type ACharacter, and must possess an APlayerController*
		FHitResult& hit: The raycast result;
		float dist: The distance from the start of the raycast to the end of it;
		FName profileName: the raycast profile name to check;
		bool bNotifyRaycastHit: Whether or not to notify any IRaycastNotifyable instances of a hit. Set to false by default;
		RETURNS bool: If the raycast hit anything, returns true.
	*/
	template<class T>
	bool RaycastSingleFromPlayerT(FHitResult& hit, float dist, FName profileName, bool bNotifyRaycastHit = false)
	{
		TArray<FHitResult> hits;
		if (RaycastMultiFromPlayerT<T>(hits, dist, profileName, bNotifyRaycastHit))
		{
			hit = hits[0];
			return true;
		}
		else return false;
	}
	/*	Performs a raycast starting at the player, towards where they are looking.
		Returns true if raycast found something. Data about raycast is stored in hit.
		Only returns true if hit actor type is the one provided.
		Parent must be of type ACharacter, and must possess an APlayerController*
		TArray<FHitResult>& hit: The raycast results;
		float dist: The distance from the start of the raycast to the end of it;
		FName profileName: the raycast profile name to check;
		bool bNotifyRaycastHit: Whether or not to notify any IRaycastNotifyable instances of a hit. Set to false by default;
		RETURNS bool: If the raycast hit anything, returns true.
	*/
	template<class T>
	bool RaycastMultiFromPlayerT(TArray<FHitResult>& hits, float dist, FName profileName, bool bNotifyRaycastHit = false)
	{
		if (RaycastMultiFromPlayer(hits, dist, profileName, bNotifyRaycastHit))
		{
			hits.RemoveAllSwap([](FHitResult& hit) -> bool
			{
				return Cast<T>(hit.GetActor()) == nullptr;
			});
			return hits.Num() > 0;
		}
		else return false;
	}
#pragma endregion
};