// Coded by Nick Lemmon

#include "../GunnerPlayer.h"
#include "../../AI/BaseAICharacter.h"
#include "../../Components/HealthComponent.h"
#include "../../Components/InventoryComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h" 
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"

AGunnerPlayer::AGunnerPlayer()
{
	playerInformation.ePlayerClass = EPlayerClass::Gunner;

	playerInformation.fActiveAbilityLength = 15.0f;
	playerInformation.fActiveAbilityCooldown = 100.0f;

	iChanceToRetrieveBullet = 40;

	//Setup the sphere component
	pShieldSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Gunner Shield Sphere"));
	pShieldSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	pShieldSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	pShieldSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
	pShieldSphere->InitSphereRadius(180.0f);
	pShieldSphere->SetupAttachment(GetCapsuleComponent());
	pShieldSphere->ComponentTags.Add("GunnerShield");
	
	//Setup shield mesh
	pShieldingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Gunner Shield Mesh"));
	pShieldingMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	pShieldingMesh->SetCollisionProfileName("NoCollision");
	pShieldingMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	pShieldingMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
	pShieldingMesh->SetupAttachment(GetCapsuleComponent());
	pShieldingMesh->SetIsReplicated(true);


	Tags.Add("Gunner");
}

bool AGunnerPlayer::UseSpecial()
{
	//Activate his defelctive shield
	EnableShield();

	return true;
}

void AGunnerPlayer::Fire()
{
	Super::Fire();

	AddToPlayerStatusFlag(EPlayerStatus::IsFiring);
	UpdatePlayerSpeed();

}

void AGunnerPlayer::StopFire()
{
	Super::StopFire();

	RemoveFromPlayerStatusFlag(EPlayerStatus::IsFiring);
	UpdatePlayerSpeed();

}

void AGunnerPlayer::AltFire()
{
	Super::AltFire();

	AddToPlayerStatusFlag(EPlayerStatus::IsFiring);
	UpdatePlayerSpeed();
}

void AGunnerPlayer::StopAltFire()
{
	Super::StopAltFire();

	RemoveFromPlayerStatusFlag(EPlayerStatus::IsFiring);
	UpdatePlayerSpeed();
}

bool AGunnerPlayer::ShieldMeshIsActive()
{
	return pShieldingMesh->GetVisibleFlag();
}

bool AGunnerPlayer::ShieldColliderIsActive()
{
	return !(pShieldSphere->GetCollisionEnabled() == ECollisionEnabled::NoCollision);
}

void AGunnerPlayer::NotifyHit(AActor* instigator, const FHitResult* hit)
{
	//TODO: Check the Actor tag or something? ¯\_(ツ)_/¯
	if (GetSpecialIsActive())
	{
		//Return the bullet
		if (hit != nullptr)
		{
			DrawDebugLine(GetWorld(), hit->ImpactPoint, hit->TraceStart, FColor::Orange, false, 5.0f);
		}
		if (instigator != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("I am returning 2.0f damage from reflected bullets"));
				UGameplayStatics::ApplyDamage(instigator,
											2.0f, 
											GetController(), 
											this, 
											UDamageType::StaticClass());

		}
		//TODO: Play a sound effect so that the player knows the bullet was absorbed or deflected

	}
}

void AGunnerPlayer::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (GetSpecialIsActive())
	{
		int RandomChance = FMath::RandRange(0,100);

		if (RandomChance <= iChanceToRetrieveBullet)
		{
			if (pInventoryComponent && pInventoryComponent->GetGun())
			{
				pInventoryComponent->GetGun()->AmmoRefill(1);
			}

			UE_LOG(LogTemp, Log, TEXT("We have given the gunner a bullet from completely random chance"));
		}

		return;
	}

	if (pHealthComponent)
	{
		pHealthComponent->HandleTakeAnyDamage(DamagedActor, Damage, DamageType, InstigatedBy, DamageCauser);
	}
}

void AGunnerPlayer::BeginPlay()
{
	Super::BeginPlay();

	DisableShield();


	//Remove the binding from our health component and rebind it to our own function
	if (pHealthComponent && OnTakeAnyDamage.IsBound())
	{
		OnTakeAnyDamage.RemoveDynamic(pHealthComponent, &UHealthComponent::HandleTakeAnyDamage);
	}
	OnTakeAnyDamage.AddDynamic(this, &AGunnerPlayer::HandleTakeAnyDamage);

}

void AGunnerPlayer::TimerEndSpecial()
{
	Super::TimerEndSpecial();
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Black, "Called the Gunner End Special");
	DisableShield();

	//Play a sound effect

}

void AGunnerPlayer::DisableShield()
{
	Server_DisableShield();
}

void AGunnerPlayer::Server_DisableShield_Implementation()
{
	//Disable Mesh
	pShieldingMesh->SetVisibility(false);

	if (pShieldSphere)
	{
		//Disable Box
		pShieldSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		pShieldSphere->SetCollisionProfileName("NoCollision");
		pShieldSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
		pShieldSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
	}
}

void AGunnerPlayer::EnableShield()
{
	Server_EnableShield();
}

void AGunnerPlayer::Server_EnableShield_Implementation()
{
	//Enable Box
	pShieldSphere->SetCollisionEnabled(CollisionEnabledType);
	pShieldSphere->SetCollisionProfileName(ActiveCollisionProfileName);
	pShieldSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	pShieldSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);

	//enable Mesh
	pShieldingMesh->SetVisibility(true);
}


