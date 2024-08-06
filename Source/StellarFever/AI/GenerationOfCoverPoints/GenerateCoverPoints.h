// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/Generators/EnvQueryGenerator_BlueprintBase.h"
#include "GenerateCoverPoints.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API UGenerateCoverPoints : public UEnvQueryGenerator_BlueprintBase
{
	GENERATED_BODY()
	
public:
	UGenerateCoverPoints(FObjectInitializer const& object_initializer);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variables")
	float Radius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variables")
	TArray<FVector> LocationSet;

	UFUNCTION(BlueprintCallable, meta = (Category = "Generator", CppFromBpEvent, OverrideNativeName = "DoItemGeneration"))
	virtual void DoItemGeneration1(TArray<FVector> const&  ContextLocations);
	void DoItemGeneration2(TArray<FVector> const& ContextLocations);

//	void DoItemGeneration(TArray<FVector> ContextLocations);
};
