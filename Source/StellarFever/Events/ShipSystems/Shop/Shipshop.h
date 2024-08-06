// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Shipshop.generated.h"

UCLASS()
class STELLARFEVER_API AShipshop : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShipshop();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/////ALL VARIABLES BELOW/////

	//The main mesh used to make the items appear in front it
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mesh")
		class UStaticMeshComponent* pMesh;

	//A Text used to display the amount of cash the ship will have
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MoneyAmount", Replicated)
		class UTextRenderComponent* pCashText;

	//A Link to the roundmanager so it knows when to activate and deactivate the shop
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Roundmanager")
		class ARoundManager* pRoundManager;

	//A Link to the shipstate so it can get the amount of cash the players have, and to affect it
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ShipState")
		class AShipState* pShipState;

	//A Link to the Medbay
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Medbay")
		class AMedicBay* pMedbay;

	//A Link to the Ammobay
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ammobay")
		class AAmmoBay* pAmmobay;

	//A subclass to spawn in a vaccine
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Subclasses")
		TSubclassOf<class AVaccineObject> pVaccineClass;

	//A subclass to spawn in an AmmoDrop
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Subclasses")
		TSubclassOf<class AAmmoDrops> pAmmoDropClass;

	//A subclass to spawn in a MedicDrop
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Subclasses")
		TSubclassOf<class AMedicDrops> pMedicDropClass;

	//The spawnpoint for pick ups and the vaccine
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawnpoint")
		class ASpawnPoint* pSpawnpoint;

	//A bool used to determien if the shop is active or not
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "IsActive", Replicated)
		bool bIsActive;

	//The list of all the potential shop items
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Items", Replicated)
		TArray<class AShopItem*> pShopItems;

	//The list of active Shop Items
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PurchasableItems", Replicated)
		TArray<AShopItem*> pPurchasableItems;

	//The number of how many items will be available, can be changed if needed
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "NumberofOptions")
		uint8 ucNumOfChoices = 4;

	/////ALL FUNCTIONS BELOW /////

	//Activates the shop to be usable
	UFUNCTION()
		void Activate();
	UFUNCTION(Server, Reliable)
		void Server_Activate();

	//Deactivates the shop so it is not usable
	UFUNCTION()
		void Deactivate();
	UFUNCTION(Server, Reliable)
		void Server_Deactivate();

	//Used to properly deactivate a bought item
	UFUNCTION()
		void PurchaseItem(AShopItem* item);
	UFUNCTION(Server, Reliable)
		void Server_PurchaseItem(AShopItem* item);

	//Sets the text of the cash counter
	UFUNCTION()
		void SetCashText(float currency);
	UFUNCTION(NetMulticast, Reliable)
		void Multicast_SetCashText(float currency);

	//Gets a random item from the array of items
	UFUNCTION()
		AShopItem* GetRandomShopItem();

	//Sets the roundmanager
	UFUNCTION()
		void SetRoundManager(ARoundManager* manager) { pRoundManager = manager; }

	//Sets the ShipState
	UFUNCTION()
		void SetShipState(AShipState* state) { pShipState = state; }
};
