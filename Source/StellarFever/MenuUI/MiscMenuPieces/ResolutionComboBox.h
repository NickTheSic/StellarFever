// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "Components/ComboBoxString.h"
#include "ResolutionComboBox.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API UResolutionComboBox : public UComboBoxString
{
	GENERATED_BODY()

public:

	TArray<FIntPoint> ScreenResolutions;


	//The init function that I don't want to call init and accidentally override something
	//This will be called from the options NativeInitialized for setup
	void CreateInitialSetup();
	
	//Creating a new version that sets up the resolution we want to use 
	UFUNCTION(BlueprintCallable, Category = "ResolutionComboBox")
		void AddResolution(const FIntPoint& NewResolution);

	UFUNCTION()
		void ChangedSelection(FString SelectedItem, ESelectInfo::Type SelectionType);


};
