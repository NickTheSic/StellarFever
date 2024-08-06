// Copyright Quarantine Games 2020


#include "DumbyFiringCharacter.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h" 
#include "Kismet/GameplayStatics.h"


// Sets default values
ADumbyFiringCharacter::ADumbyFiringCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->SetVisibility(true);

	raycast = CreateDefaultSubobject<URaycastComponent>(TEXT("Raycaster"));
}

// Called when the game starts or when spawned
void ADumbyFiringCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADumbyFiringCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	timer += DeltaTime;
	if (timer > 2.0f)
	{
		timer = 0.0f;

		FHitResult hit;
		if (raycast->RaycastSingle(hit, GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 500.0f,
			ECollisionChannel::ECC_MAX, true))
		{
			if (hit.GetActor() && GetLocalRole() == ROLE_Authority)
			{
				UGameplayStatics::ApplyDamage(hit.GetActor(), 5, 
					GetController(), this, UDamageType::StaticClass());

				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Emerald, "I hit player" + hit.GetActor()->GetActorLabel());
			}
		}
		DrawDebugLine(GetWorld(), hit.TraceStart, hit.TraceEnd, FColor::Magenta, false, 1.0f);
		
	}
}
