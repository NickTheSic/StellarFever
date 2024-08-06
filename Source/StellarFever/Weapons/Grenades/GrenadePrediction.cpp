// Copyright Quarantine Games 2020


#include "GrenadePrediction.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AGrenadePrediction::AGrenadePrediction()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//do not collide with anything
	pMesh = CreateDefaultSubobject<UStaticMeshComponent>("Prediction Mesh");
	pMesh->SetCollisionProfileName("NoCollision");
	pMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel11, ECollisionResponse::ECR_Ignore);
	pMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel12, ECollisionResponse::ECR_Ignore);
	pMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	pMesh->SetSimulatePhysics(false);
	pMesh->SetIsReplicated(true);

	SetRootComponent(pMesh);

	bReplicates = true;
	SetReplicates(true);
	SetReplicateMovement(true);

	SetActorHiddenInGame(true);
	pMesh->SetHiddenInGame(true);
}

// Called when the game starts or when spawned
void AGrenadePrediction::BeginPlay()
{
	Super::BeginPlay();
}

void AGrenadePrediction::SetMeshComp_Implementation(UStaticMesh* mesh)
{
	pMesh->SetStaticMesh(mesh);
}

void AGrenadePrediction::SetMeshMat_Implementation(UMaterial* mat)
{
	for (int i = 0; i < pMesh->GetNumMaterials(); i++)
		pMesh->SetMaterial(i, mat);
}

void AGrenadePrediction::HidePath_Implementation()
{
	SetActorHiddenInGame(true);
	pMesh->SetHiddenInGame(true);
}

void AGrenadePrediction::ShowPath_Implementation()
{
	SetActorHiddenInGame(false);
	pMesh->SetHiddenInGame(false);
}

void AGrenadePrediction::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGrenadePrediction, pMesh);
}