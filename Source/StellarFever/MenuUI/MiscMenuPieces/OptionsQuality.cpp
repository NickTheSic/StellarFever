// Copyright Quarantine Games 2020


#include "OptionsQuality.h"
#include "Components/ComboBoxString.h"
#include "Kismet/GameplayStatics.h"


void UOptionsQuality::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (QualityBox)
	{
		QualityBox->AddOption("Low");
		QualityBox->AddOption("Medium");
		QualityBox->AddOption("High");
		QualityBox->AddOption("Epic");
		QualityBox->AddOption("Cinematic");

		QualityBox->OnSelectionChanged.AddDynamic(this, &UOptionsQuality::ComboBoxSetQuality);
	}
}


void UOptionsQuality::ComboBoxSetQuality(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	//Store the current value of our quality level
	uint32 NewValue = uiQualityLevel;

	if (SelectedItem.Equals("Low"))
		NewValue = 0;
	if (SelectedItem.Equals("Medium"))
		NewValue = 1;
	if (SelectedItem.Equals("High"))
		NewValue = 2;
	if (SelectedItem.Equals("Epic"))
		NewValue = 3;
	if (SelectedItem.Equals("Cinematic"))
		NewValue = 4;

	//If we have a different new value then set the resolution
	if (uiQualityLevel != NewValue)
	{
		uiQualityLevel = FMath::Clamp<uint32>(NewValue, 0, 4);
		OnSetQuality.ExecuteIfBound(uiQualityLevel);
	}
}


void UOptionsQuality::SetQualityLevel(uint32 value)
{
	uiQualityLevel = FMath::Clamp<uint32>(value, 0, 4);

	QualityBox->SetSelectedIndex(uiQualityLevel);
}

