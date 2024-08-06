// Copyright Quarantine Games 2020

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ServerInfoRow.generated.h"

/**
 * 
 */
UCLASS()
class STELLARFEVER_API UServerInfoRow : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* ServerNameText = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* HostUserText = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* ConnectionFractionText = nullptr;


	void Setup(class UJoinMenu* parent, uint32 index );


	bool bSelected = false;


	UPROPERTY(meta = (BindWidget))
		class UButton* RowButton = nullptr;

private:
	
	class UJoinMenu* pJoinMenu = nullptr;

	uint32 uiIndex;

	UFUNCTION()
		void OnClickRowButton();

};
