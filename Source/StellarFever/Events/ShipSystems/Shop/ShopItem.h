// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShopItem.generated.h"

	//The type of item it will be, an enum of the types
UENUM()
enum ItemType
{
	Ammobay,
	Medbay,
	Fuel,
	Vaccine,
	AmmoDrop,
	MedDrop,
};

UCLASS()
class STELLARFEVER_API AShopItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShopItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/////ALL VARIABLES BELOW/////

	//The main mesh for the shop to appear on
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mesh")
		class UStaticMeshComponent* pMesh;

	//The main collider that is interacted with
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Collider")
		class UBoxComponent* pCollider;

	//The cost of the item
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Cost", Replicated)
		float fCost;

	//The min random cost of the item
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Cost")
		uint8 ucMinCost;

	//The max random cost of the item
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Cost")
		uint8 ucMaxCost;

	//Which type of Item is this specific instance
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ItemType", Replicated)
		TEnumAsByte<ItemType> itemtype;

	//The shop that owns the items,
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shipshop")
		class AShipshop* pShop;

	//If the Item is active or not
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "isactive", Replicated)
		bool bIsActive;

	//The text for the name
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ItemText", Replicated)
		class UTextRenderComponent* pNameText;

	//The text for the cost
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ItemText", Replicated)
		UTextRenderComponent* pCostText;

	/////ALL FUNCTIONS BELOW/////

	//Activation and Deactivation for the objects
	UFUNCTION()
		void Activate();
	UFUNCTION(Server, Reliable)
		void Server_Activate();

	//Deactivate is used specifically for activating a level/round, when interacted by the player
	UFUNCTION()
		void Deactivate();
	UFUNCTION(Server, Reliable)
		void Server_Deactivate();

	//Used when purchsing an item, called when interacted with
	UFUNCTION()
		void Disable();
	UFUNCTION(Server, Reliable)
		void Server_Disable();

	//Making the text visible and invisible when near the planet select
	UFUNCTION()
		void ActivateInfo();
	UFUNCTION(Server, Reliable)
		void Server_ActivateInfo();

	UFUNCTION()
		void DeactivateInfo();
	UFUNCTION(Server, Reliable)
		void Server_DeactivateInfo();

	//Sets the proper info for each text block
	UFUNCTION()
		void SetupShopText();
	UFUNCTION(NetMulticast, Reliable)
		void Multicast_SetupShopText();

	//Gets and sets for the values from the item
	float GetCost() { return fCost; }

	ItemType GetType() { return itemtype; }

	bool GetIsActive() { return bIsActive; }

	void SetIsActive(bool isactive) { bIsActive = isactive; }

	void SetShop(AShipshop* shop) { pShop = shop; }
};
