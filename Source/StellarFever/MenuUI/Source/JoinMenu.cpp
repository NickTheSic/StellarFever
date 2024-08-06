// Copyright Quarantine Games 2020


#include "../JoinMenu.h"
#include "../MainMenuSwitcher.h"
#include "../MiscMenuPieces/ServerInfoRow.h"
#include "../../StellarFeverGameInstance.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"

void UJoinMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (BackButton)
	{
		BackButton->OnClicked.AddDynamic(this, &UJoinMenu::OnClickBack);
		_Buttons.Add(BackButton);
	}

	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &UJoinMenu::JoinServer);
		_Buttons.Add(JoinButton);
	}

	if (RefreshButton)
	{
		RefreshButton->OnClicked.AddDynamic(this, &UJoinMenu::RefreshServers);
		_Buttons.Add(RefreshButton);
	}

	UStellarFeverGameInstance* sfInstance = Cast<UStellarFeverGameInstance>(GetWorld()->GetGameInstance());
	if (sfInstance)
	{
		sfInstance->pJoinMenu = this;
	}
}


void UJoinMenu::SetServerList(TArray<FServerData> ServerNames)
{
	UWorld* World = this->GetWorld();
	if (!ensure(World != nullptr)) return;

	if (ServerRowTemplate == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Server Row Template was null, returning"));
		return;
	}

	//Remove the buttons from the hover list
	for (int i = 0; i < ServerList->GetChildrenCount(); i++)
	{
		UServerInfoRow* row = Cast<UServerInfoRow>(ServerList->GetChildAt(i));

		if (row)
		{
			if (_Buttons.Contains(row->RowButton))
			{
				_Buttons.Remove(row->RowButton);
			}
		}
	}

	ServerList->ClearChildren();

	uint32 i = 0;
	for (const FServerData& ServerData : ServerNames)
	{
		UServerInfoRow* Row = CreateWidget<UServerInfoRow>(World, ServerRowTemplate);
		if (!ensure(Row != nullptr)) return;


		Row->ServerNameText->SetText(FText::FromString(ServerData.Name));
		Row->HostUserText->SetText(FText::FromString(ServerData.HostUsername));
		FString FractionText = FString::Printf(TEXT("%d/%d"), ServerData.CurrentPlayers, ServerData.MaxPlayers);
		Row->ConnectionFractionText->SetText(FText::FromString(FractionText));
		Row->Setup(this, i);
		++i;

		ServerList->AddChild(Row);

		//Set the focus to the button
		//If there are multiple servers it will pick the last
		if (Row && Row->RowButton)
		{
			Row->RowButton->SetFocus();
			_Buttons.Add(Row->RowButton);
		}
	}
}


void UJoinMenu::OnClickBack()
{
	if (pMainMenuSwitcher)
	{
		pMainMenuSwitcher->SwitchToMainMenu();
	}
}


void UJoinMenu::JoinServer()
{
	if (SelectedIndex.IsSet())
	{
		if (UStellarFeverGameInstance* sfInstance = Cast<UStellarFeverGameInstance>(GetGameInstance()))
		{
			sfInstance->Join(SelectedIndex.GetValue());

			pMainMenuSwitcher->SwitchToLoadingScreen();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected index wasn't set"));
	}
}


void UJoinMenu::RefreshServers()
{
	UStellarFeverGameInstance* sfInstance = Cast<UStellarFeverGameInstance>(GetWorld()->GetGameInstance());
	if (sfInstance)
	{
		sfInstance->RefreshServerList();
	}
}


void UJoinMenu::SelectIndex(uint32 index)
{
	SelectedIndex = index;
	UpdateChildren();
}


void UJoinMenu::UpdateChildren()
{
	for (int32 i = 0; i < ServerList->GetChildrenCount(); ++i)
	{
		auto Row = Cast<UServerInfoRow>(ServerList->GetChildAt(i));
		if (Row != nullptr)
		{
			Row->bSelected = (SelectedIndex.IsSet() && SelectedIndex.GetValue() == i);
		}
	}
}
