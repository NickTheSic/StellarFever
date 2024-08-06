// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickUpItem.generated.h"

UCLASS()
class STELLARFEVER_API APickUpItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickUpItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	/////ALL VARIABLES BEELOW/////

	//Main mesh for object
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh", Replicated)
		class UStaticMeshComponent* pMesh;

	//Whether is object is active or not
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IsActive", Replicated)
		bool bIsActive;

	//Speed of when the object will be thrown
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwspeed")
		float fThrowSpeed = 5000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals")
		float fDistanceForRenderOutline = 5000.f;

public:
	/////ALL FUNCTIONS BELOW/////

		//Called when created to establish basic functionality
	UFUNCTION()
		virtual void Activate();
	UFUNCTION(Server, Reliable)
		virtual void Server_Activate();
	UFUNCTION(NetMulticast, Reliable)
		virtual void Multicast_Activate();

	//Called when destroyed when in the proper location
	UFUNCTION()
		virtual void Deactivate();
	UFUNCTION(Server, Reliable)
		virtual void Server_Deactivate();
	UFUNCTION(NetMulticast, Reliable)
		virtual void Multicast_Deactivate();

	//Called when the object is pickedup to bind it to the right socket
	UFUNCTION()
		virtual void PickUpObject();
	UFUNCTION(NetMulticast, Reliable)
		virtual void Multicast_PickUpObject();

	//Called when the object is dropped
	UFUNCTION()
		virtual void DropObject(FVector direction);
	UFUNCTION(NetMulticast, Reliable)
		virtual void Multicast_DropObject(FVector direction);

	//A Getter if the pickup is active or not
	UFUNCTION()
		bool GetIsActive() { return bIsActive; }

};
