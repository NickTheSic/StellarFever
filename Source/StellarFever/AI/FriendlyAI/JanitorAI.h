// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "JanitorAI.generated.h"

class USkeletalMeshComponent;
class UBehaviorTree;
class APatrolPath;
class UBlackboardComponent;
class UAudioComponent;
class USoundCue;
class AJanitorAI_AIController;
//
//UENUM(BlueprintType)
//enum StatusJanitor
//{
//	Patrolling   UMETA(DisplayName = "Patrolling"),
//	PickedUp      UMETA(DisplayName = "PickedUp"),
//	Fixing		  UMETA(DisplayName = "Fixing"),
//};


UCLASS()
class STELLARFEVER_API AJanitorAI : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AJanitorAI();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USkeletalMeshComponent* pMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	AJanitorAI_AIController* pController;

	UPROPERTY()
	UBlackboardComponent* BlackboardComponent;

	UPROPERTY(EditAnywhere, Category = "AI")
		float fMovementSpeed;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	//	TEnumAsByte<StatusJanitor> eStatus;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
	APatrolPath* get_secure_patrol_path;
	APatrolPath* get_patrol_path();

	TArray<AActor*> SecurePaths;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger")
	APatrolPath* patrol_path;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sound")
		UAudioComponent* pBallAudioComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
		USoundCue* pBallRollSound;

	UPROPERTY()
	bool bBallRoll;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dunno")
	bool bPickedUp;

	//UFUNCTION(Server, Reliable)
	void ServerSound();

	/////ALL FUNCTIONS BELOW/////
	void Activate();
	void Deactivate();


	//UFUNCTION()
	//void Enable(); //Meant for toilet event
	//UFUNCTION()
	//void Disable(); //Meant for toilet event

	//bool bDisabled; //Meant for toilet event

	////Ask ROB!!!!!!
	//bool bIsFixing; //make it so he cannot be distracted? picked up? dunno yet

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
