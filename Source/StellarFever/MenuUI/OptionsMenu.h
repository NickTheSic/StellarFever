// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "MiscMenuPieces/ControllerHoverButtonWidget.h"
#include "OptionsMenu.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API UOptionsMenu : public UControllerHoverButtonWidget
{
	GENERATED_BODY()

public:

	/*Overridden from UUserWidget*/
	void NativeOnInitialized() override;
	void NativeConstruct() override;


	/**Main Menu Items*/
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UTextBlock* TitleText = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* BackButton = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* ApplySettingsButton = nullptr;


	/**Common Menu Functions*/
	UFUNCTION()
		void OnClickBackButton();
	UFUNCTION()
		void SaveSettings();
	UFUNCTION()
		void OnApplySettingsClicked();


	/**Options Switching Buttons*/
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* VideoSettingsButton = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* AudioSettingsButton = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* ControllerSettingsButton = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UWidgetSwitcher* OptionMenuTabs = nullptr;


	/**Tab options*/
	UFUNCTION()
		void SwitchToVideoSettings();
	UFUNCTION()
		void SwitchToAudioSettings();
	UFUNCTION()
		void SwitchToControllerSettings();


	/**Video Options*/
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UCanvasPanel* VideoSettings = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UCheckBox* VSyncCheckBox = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UComboBoxString* WindowModeComboBox = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UResolutionComboBox* ResolutionComboBox = nullptr;


	/**Quality options*/
	UPROPERTY(meta = (BindWidget))
		class UOptionsQuality* AntiAliasingQuality;
	UPROPERTY(meta = (BindWidget))
		class UOptionsQuality* ShadowQuality;
	UPROPERTY(meta = (BindWidget))
		class UOptionsQuality* ShadingQuality;
	UPROPERTY(meta = (BindWidget))
		class UOptionsQuality* TextureQuality;
	UPROPERTY(meta = (BindWidget))
		class UOptionsQuality* ViewDistanceQuality;


	/**Brightness*/
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UTextBlock* BrightnessLabel = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UTextBlock* BrightnessValue = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class USlider* BrightnessSlider = nullptr;


	/**Audio Menu*/
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UCanvasPanel* AudioSettings = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UTextBlock* GeneralVolLabel = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UTextBlock* GeneralVolValue = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class USlider* GeneralVolSlider = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UTextBlock* VoiceVolLabel = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UTextBlock* VoiceVolValue = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class USlider* VoiceVolSlider = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UTextBlock* MusicVolLabel = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UTextBlock* MusicVolValue = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class USlider* MusicVolSlider = nullptr;

	
	/**Controller Menu*/
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UCanvasPanel* ControllerSettings = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UTextBlock* LookSensitivityLabel = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UTextBlock* LookSensitivityValue = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class USlider* LookSensitivitySlider = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UComboBoxString* InvertAxisComboBox = nullptr;

	/**
	* Go to here for use about UInput settings for when the controls need to be rebound
	*https://docs.unrealengine.com/en-US/API/Runtime/Engine/GameFramework/UInputSettings/index.html
	*/


	/**Pointer to other*/
	class UMainMenuSwitcher* pMainMenuSwitcher = nullptr;
	class UPauseMenu* pPauseMenuSwitcher = nullptr;

	class UGameUserSettings* pCurrentGameUserSettings = nullptr;


	/**Functions*/
	UFUNCTION()
		void OnBrightnessValueChanged(float value);
	UFUNCTION()
		void OnGeneralVolumeValueChanged(float value);
	UFUNCTION()
		void OnMusicVolumeValueChanged(float value);
	UFUNCTION()
		void OnVoiceVolumeValueChanged(float value);
	UFUNCTION()
		void OnLookSensitivityValueChanged(float value);
	UFUNCTION()
		void OnToggleVSync(bool bIsChecked);
	UFUNCTION()
		void SetWindowMode(FString SelectedItem, ESelectInfo::Type SelectionType);

};
