// Copyright Quarantine Games 2020


#include "PredictionPath.h"
#include "GrenadePrediction.h"
#include "Net/UnrealNetwork.h"
#include "../GrenadeBase.h"

// Sets default values for this component's properties
UPredictionPath::UPredictionPath()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
    SetIsReplicatedByDefault(true);
}


// Called when the game starts
void UPredictionPath::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UPredictionPath::SpawnPath_Implementation(uint8 PredictNum, UStaticMesh* pGrenadePredictionExample, UMaterial* pPredictMaterial, AGrenadeBase* grenade)
{
    //based on predict num, spawn and set prediction actors
    for (int i = 0; i < PredictNum; i++)
    {
        AGrenadePrediction* temp = GetWorld()->SpawnActor<AGrenadePrediction>(FVector(0, 0, -20000), FRotator::ZeroRotator);
        temp->SetOwner(grenade);
        temp->SetMeshComp(pGrenadePredictionExample);
        temp->SetMeshMat(pPredictMaterial);
        GrenadePredictionArray.Add(temp);
    }
}

void UPredictionPath::Client_ShowPath_Implementation()
{
    for (int i = 0; i < GrenadePredictionArray.Num(); i++)
        GrenadePredictionArray[i]->ShowPath();
}

void UPredictionPath::Client_ShowPathSingle_Implementation(uint8 pathNum)
{
    GrenadePredictionArray[pathNum]->ShowPath();
}

void UPredictionPath::Client_HidePath_Implementation()
{
	for (int i = 0; i < GrenadePredictionArray.Num(); i++)
	{
		if (GrenadePredictionArray[i] != nullptr)
		{
			GrenadePredictionArray[i]->HidePath();
		}
	}
}

void UPredictionPath::Client_HidePathSingle_Implementation(uint8 pathNum)
{
    GrenadePredictionArray[pathNum]->HidePath();
}

TArray<AGrenadePrediction*> UPredictionPath::GetPredictionArray()
{
    return GrenadePredictionArray;
}

void UPredictionPath::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UPredictionPath, GrenadePredictionArray);

}