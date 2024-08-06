// Copyright Quarantine Games 2020


#include "Service_ChangeSpeed.h"
#include "..\BaseAICharacter.h"
#include "..\BaseAICharacter_AIController.h"
#include "GameFramework/CharacterMovementComponent.h"

UService_ChangeSpeed::UService_ChangeSpeed()
{
	bNotifyBecomeRelevant = true;
	NodeName = TEXT("Change Speed C++");
}

void UService_ChangeSpeed::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
	ABaseAICharacter_AIController* controller = Cast<ABaseAICharacter_AIController>(OwnerComp.GetAIOwner());
	ABaseAICharacter* aiPawn = Cast<ABaseAICharacter>(controller->GetPawn());

	if (bWalking)
	{
		fSpeed = aiPawn->fAIMovementSpeed / 4.f;
	}
	else
	{
		fSpeed = aiPawn->fAIMovementSpeed;
	}

	aiPawn->GetCharacterMovement()->MaxWalkSpeed = fSpeed;

}

FString UService_ChangeSpeed::GetStaticServiceDescription() const
{
	return FString("Change the AIs speed");
}
