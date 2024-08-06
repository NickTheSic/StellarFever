// Copyright Quarantine Games 2020


#include "ResolutionComboBox.h"
#include "GameFramework/GameUserSettings.h"

void UResolutionComboBox::CreateInitialSetup()
{
	//Add resolutions
	AddResolution(FIntPoint(1920, 1080));
	AddResolution(FIntPoint(1680, 1050));
	AddResolution(FIntPoint(1600, 900));
	AddResolution(FIntPoint(1440, 900));
	AddResolution(FIntPoint(1280, 800));
	AddResolution(FIntPoint(1280, 720));
	AddResolution(FIntPoint(1136, 640));


	//Set current index of the resolution if it is found
	FIntPoint point = UGameUserSettings::GetGameUserSettings()->GetScreenResolution();

	int32 indexFound = INDEX_NONE;
	if (ScreenResolutions.Find(point, indexFound))
	{
		SetSelectedIndex(indexFound);
	}

	//Bind the delegate
	OnSelectionChanged.AddDynamic(this, &UResolutionComboBox::ChangedSelection);
}

void UResolutionComboBox::AddResolution(const FIntPoint& NewResolution)
{
	//Add the new resolution to an array
	ScreenResolutions.Add(NewResolution);

	//Add the option to the menu
	FString ReferenceString = FString::FromInt(NewResolution.X) + "x" + FString::FromInt(NewResolution.Y);
	AddOption(ReferenceString);
}

void UResolutionComboBox::ChangedSelection(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	//Get the index of the resolution from the selected options
	int32 index = FindOptionIndex(SelectedItem);
	
	//Set the resolution to the same index as in our Screen Resolutions
	if(ScreenResolutions.IsValidIndex(index))
		UGameUserSettings::GetGameUserSettings()->SetScreenResolution(ScreenResolutions[index]) ;
}
