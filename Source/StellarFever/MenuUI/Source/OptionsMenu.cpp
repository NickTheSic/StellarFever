// Copyright Quarantine Games 2020

#include "../OptionsMenu.h"
#include "../MainMenuSwitcher.h"
#include "../MiscMenuPieces/OptionsQuality.h"
#include "../MiscMenuPieces/ResolutionComboBox.h"
#include "../../SFBasicSaveGame.h"
#include "../../StellarFeverGameInstance.h"
#include "../../PauseMenu/PauseMenu.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"
#include "../../Player/SF_PlayerController.h"


FString TruncateFloatToString(float ff)
{
	FString f = FString::SanitizeFloat(ff);
	FJsonSerializableArray arr;
	f.ParseIntoArray(arr, TEXT("."));
	if (arr.Num() > 1)
	{
		arr[1] = arr[1].LeftChop(4);
		if (arr[1] == "")
			arr[1] = "00";
		return arr[0] + "." + arr[1];
	}
	else
		return f + ".00";

}

#undef QUALITY_SETUP
#define QUALITY_SETUP(name)\
 if(name)\
{\
name->QualityText->SetText( FText::FromString( #name ) );\
name->SetQualityLevel(pCurrentGameUserSettings->Get##name()); \
name->OnSetQuality.BindDynamic(pCurrentGameUserSettings, &UGameUserSettings::Set##name);\
}\

void UOptionsMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();


	pCurrentGameUserSettings = UGameUserSettings::GetGameUserSettings();


	if (BackButton)
	{
		BackButton->OnClicked.AddDynamic(this, &UOptionsMenu::OnClickBackButton);
		_Buttons.Add(BackButton);
	}

	if (ApplySettingsButton)
	{
		ApplySettingsButton->OnClicked.AddDynamic(this, &UOptionsMenu::OnApplySettingsClicked);
		_Buttons.Add(ApplySettingsButton);
	}

	if (VSyncCheckBox)
	{
		VSyncCheckBox->SetIsChecked(pCurrentGameUserSettings->IsVSyncEnabled());
		VSyncCheckBox->OnCheckStateChanged.AddDynamic(this, &UOptionsMenu::OnToggleVSync);
	}

	if (WindowModeComboBox)
	{
		WindowModeComboBox->AddOption("Fullscreen");
		WindowModeComboBox->AddOption("WindowedFullscreen");
		WindowModeComboBox->AddOption("Windowed");

		//Set the selected index to the current value of the current window mode
		WindowModeComboBox->SetSelectedIndex(TEnumAsByte<EWindowMode::Type>(pCurrentGameUserSettings->GetFullscreenMode()).GetValue());

		WindowModeComboBox->OnSelectionChanged.AddDynamic(this, &UOptionsMenu::SetWindowMode);
	}

	if (ResolutionComboBox)
	{
		ResolutionComboBox->CreateInitialSetup();
	}

	if (VideoSettingsButton)
	{
		VideoSettingsButton->OnClicked.AddDynamic(this, &UOptionsMenu::SwitchToVideoSettings);
		_Buttons.Add(VideoSettingsButton);
	}

	if (AudioSettingsButton)
	{
		AudioSettingsButton->OnClicked.AddDynamic(this, &UOptionsMenu::SwitchToAudioSettings);
		_Buttons.Add(AudioSettingsButton);
	}

	if (ControllerSettingsButton)
	{
		ControllerSettingsButton->OnClicked.AddDynamic(this, &UOptionsMenu::SwitchToControllerSettings);
		_Buttons.Add(ControllerSettingsButton);
	}


	QUALITY_SETUP(AntiAliasingQuality);
	QUALITY_SETUP(ShadowQuality);
	QUALITY_SETUP(ShadingQuality);
	QUALITY_SETUP(TextureQuality);
	QUALITY_SETUP(ViewDistanceQuality);


	UStellarFeverGameInstance* sfInstance = Cast<UStellarFeverGameInstance>(GetWorld()->GetGameInstance());
	if (sfInstance)
	{
		if (!sfInstance->pSaveData)
			sfInstance->LoadGame();

		if (BrightnessSlider && BrightnessValue)
		{
			BrightnessSlider->SetValue(sfInstance->fBrightness);
			BrightnessValue->SetText(FText::FromString(TruncateFloatToString(sfInstance->fBrightness) + "%"));
		}

		if (LookSensitivitySlider && LookSensitivityValue)
		{
			LookSensitivitySlider->SetValue(sfInstance->fLookSensitivity);
			LookSensitivityValue->SetText(FText::FromString(TruncateFloatToString(sfInstance->fLookSensitivity)));
		}

		if (GeneralVolSlider && GeneralVolValue)
		{
			GeneralVolSlider->SetValue(sfInstance->fGeneralVolume);
			GeneralVolValue->SetText(FText::FromString(TruncateFloatToString(sfInstance->fGeneralVolume)));
		}
		if (MusicVolSlider && MusicVolValue)
		{
			MusicVolSlider->SetValue(sfInstance->fMusicVolume);
			MusicVolValue->SetText(FText::FromString(TruncateFloatToString(sfInstance->fMusicVolume)));
		}
		if (VoiceVolSlider && VoiceVolValue)
		{
			VoiceVolSlider->SetValue(sfInstance->fVoiceVolume);
			VoiceVolValue->SetText(FText::FromString(TruncateFloatToString(sfInstance->fVoiceVolume)));
		}
	}
	else
	{

		if (BrightnessSlider && BrightnessValue)
		{
			BrightnessSlider->SetValue(50.f);
			BrightnessValue->SetText(FText::FromString("50%"));
		}

		if (LookSensitivitySlider && LookSensitivityValue)
		{
			LookSensitivitySlider->SetValue(1.0f);
			LookSensitivityValue->SetText(FText::FromString("1.0"));
		}

		if (GeneralVolSlider && GeneralVolValue)
		{
			GeneralVolSlider->SetValue(50.f);
			GeneralVolValue->SetText(FText::FromString("50%"));
		}
		if (MusicVolSlider && MusicVolValue)
		{
			MusicVolSlider->SetValue(50.f);
			MusicVolValue->SetText(FText::FromString("50%"));
		}
		if (VoiceVolSlider && VoiceVolValue)
		{
			VoiceVolSlider->SetValue(50.f);
			VoiceVolValue->SetText(FText::FromString("50%"));
		}
	}


	if (BrightnessSlider && BrightnessValue)
	{
		BrightnessSlider->OnValueChanged.AddDynamic(this, &UOptionsMenu::OnBrightnessValueChanged);
	}

	if (LookSensitivitySlider && LookSensitivityValue)
	{
		LookSensitivitySlider->OnValueChanged.AddDynamic(this, &UOptionsMenu::OnLookSensitivityValueChanged);
	}

	if (GeneralVolSlider && GeneralVolValue)
	{
		GeneralVolSlider->OnValueChanged.AddDynamic(this, &UOptionsMenu::OnGeneralVolumeValueChanged);
	}
	if (MusicVolSlider && MusicVolValue)
	{
		MusicVolSlider->OnValueChanged.AddDynamic(this, &UOptionsMenu::OnMusicVolumeValueChanged);
	}
	if (VoiceVolSlider && VoiceVolValue)
	{
		VoiceVolSlider->OnValueChanged.AddDynamic(this, &UOptionsMenu::OnVoiceVolumeValueChanged);
	}
}
#undef QUALITY_SETUP

void UOptionsMenu::NativeConstruct()
{
	Super::NativeConstruct();

	UStellarFeverGameInstance* sfInstance = Cast<UStellarFeverGameInstance>(GetWorld()->GetGameInstance());
	if (BrightnessSlider && BrightnessValue)
	{
		BrightnessSlider->SetValue(sfInstance->fBrightness);
		BrightnessValue->SetText(FText::FromString(TruncateFloatToString(sfInstance->fBrightness) + "%"));
	}

	if (LookSensitivitySlider && LookSensitivityValue)
	{
		LookSensitivitySlider->SetValue(sfInstance->fLookSensitivity);
		LookSensitivityValue->SetText(FText::FromString(TruncateFloatToString(sfInstance->fLookSensitivity)));
	}

	if (GeneralVolSlider && GeneralVolValue)
	{
		GeneralVolSlider->SetValue(sfInstance->fGeneralVolume);
		GeneralVolValue->SetText(FText::FromString(TruncateFloatToString(sfInstance->fGeneralVolume)));
	}
	if (MusicVolSlider && MusicVolValue)
	{
		MusicVolSlider->SetValue(sfInstance->fMusicVolume);
		MusicVolValue->SetText(FText::FromString(TruncateFloatToString(sfInstance->fMusicVolume)));
	}
	if (VoiceVolSlider && VoiceVolValue)
	{
		VoiceVolSlider->SetValue(sfInstance->fVoiceVolume);
		VoiceVolValue->SetText(FText::FromString(TruncateFloatToString(sfInstance->fVoiceVolume)));
	}
	if (InvertAxisComboBox != nullptr)
	{
		InvertAxisComboBox->SetSelectedIndex((int)sfInstance->eInvertAxis);
	}
}

void UOptionsMenu::OnClickBackButton()
{
	if (pMainMenuSwitcher != nullptr)
		pMainMenuSwitcher->SwitchToMainMenu();

	else if (pPauseMenuSwitcher != nullptr)
	{
		pPauseMenuSwitcher->SwitchToMainMenu();
	}
}

void UOptionsMenu::SaveSettings()
{
	UStellarFeverGameInstance* sfInstance = GetWorld()->GetGameInstance<UStellarFeverGameInstance>();
	if (sfInstance)
	{
		sfInstance->fBrightness = BrightnessSlider->GetValue();
		sfInstance->fLookSensitivity = LookSensitivitySlider->GetValue();
		if (GetWorld()->GetMapName().Mid(GetWorld()->StreamingLevelsPrefix.Len()) != "MainMenuMap")
			GetOwningPlayerPawn()->GetController<ASF_PlayerController>()->UpdateLookSpeed(sfInstance->fLookSensitivity);
		sfInstance->UpdateVolume(GeneralVolSlider->GetValue(), MusicVolSlider->GetValue(), VoiceVolSlider->GetValue());
		
		sfInstance->eInvertAxis = (EInvertControllerAxis)InvertAxisComboBox->GetSelectedIndex();
		sfInstance->SaveGame();
	}
}

void UOptionsMenu::OnApplySettingsClicked()
{
	if (pCurrentGameUserSettings)
		pCurrentGameUserSettings->ApplySettings(false);

	SaveSettings();
}

void UOptionsMenu::SwitchToVideoSettings()
{
	if (OptionMenuTabs)
	{
		OptionMenuTabs->SetActiveWidget(VideoSettings);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Options Menu Widget switcher was null"));
	}
}

void UOptionsMenu::SwitchToAudioSettings()
{
	if (OptionMenuTabs)
	{
		OptionMenuTabs->SetActiveWidget(AudioSettings);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Options Menu Widget switcher was null"));
	}
}

void UOptionsMenu::SwitchToControllerSettings()
{
	if (OptionMenuTabs)
	{
		OptionMenuTabs->SetActiveWidget(ControllerSettings);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Options Menu Widget switcher was null"));
	}
}

void UOptionsMenu::OnToggleVSync(bool bIsChecked)
{
	pCurrentGameUserSettings->SetVSyncEnabled(bIsChecked);
}

void UOptionsMenu::SetWindowMode(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (SelectedItem.Equals("Fullscreen"))
	{
		pCurrentGameUserSettings->SetFullscreenMode(EWindowMode::Fullscreen);
	}	
	else if (SelectedItem.Equals("WindowedFullscreen"))
	{
		pCurrentGameUserSettings->SetFullscreenMode(EWindowMode::WindowedFullscreen);
	}
	else if (SelectedItem.Equals("Windowed"))
	{
		pCurrentGameUserSettings->SetFullscreenMode(EWindowMode::Windowed);
	}
}

void UOptionsMenu::OnBrightnessValueChanged(float value)
{
	value = FMath::Clamp(value, 1.0f, 100.f);
	BrightnessValue->SetText(FText::FromString(TruncateFloatToString(value) + "%"));
	Cast<APlayerController>(GetOwningPlayerPawn()->Controller)->ConsoleCommand("gamma " + FString::SanitizeFloat(value / 100.f * 4.5f + 0.5f));
}

void UOptionsMenu::OnLookSensitivityValueChanged(float value)
{
	value = FMath::Clamp(value, .25f, 10.f);
	LookSensitivityValue->SetText(FText::FromString(TruncateFloatToString(value)));
}


void UOptionsMenu::OnGeneralVolumeValueChanged(float value)
{
	value = FMath::Clamp(value, 0.0f, 100.f);
	GeneralVolValue->SetText(FText::FromString(TruncateFloatToString(value) + "%"));
}

void UOptionsMenu::OnMusicVolumeValueChanged(float value)
{
	value = FMath::Clamp(value, 0.0f, 100.f);
	MusicVolValue->SetText(FText::FromString(TruncateFloatToString(value) + "%"));
}

void UOptionsMenu::OnVoiceVolumeValueChanged(float value)
{
	value = FMath::Clamp(value, 0.0f, 100.f);
	VoiceVolValue->SetText(FText::FromString(TruncateFloatToString(value) + "%"));
}

