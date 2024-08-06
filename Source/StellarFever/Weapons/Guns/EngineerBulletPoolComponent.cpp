// Copyright Quarantine Games 2020


#include "EngineerBulletPoolComponent.h"
#include "EngineerBullet.h"
#include "EngineerGun.h"
#include "Sound/SoundCue.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraSystem.h"

// Sets default values for this component's properties
UEngineerBulletPoolComponent::UEngineerBulletPoolComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
	ucActiveBullets = 0;

	SetIsReplicatedByDefault(true);

}


// Called when the game starts
void UEngineerBulletPoolComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UEngineerBulletPoolComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UEngineerBulletPoolComponent::Server_Init_Implementation(uint8 PoolSize, AEngineerGun* engineerGun)
{
	//if (engineerGun->GetOwner()->GetLocalRole() == ROLE_Authority)
	//{
		//create PoolSize amount of bullets, and add them to the array
		for (int i = 0; i < PoolSize; i++)
		{
			//setup bullet pool
			AEngineerBullet* TempBullet = GetWorld()->SpawnActor<AEngineerBullet>(FVector(0, 0, -20000), FRotator::ZeroRotator);
			TempBullet->SetOwner(engineerGun);
			TempBullet->SetCameraShake(engineerGun->GetBulletCameraShake());
			Bullets.Add(TempBullet);
		}
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Bullet Pool Initialized");
	//}
}

void UEngineerBulletPoolComponent::Server_Fire_Implementation(const FHitResult& hit, const FQuat& dir, float BurnDamage, float BurnDuration, float HitsPerSecond, float ExplosionDamage)
{
	int BulletIndex = -1;		//used to find the unfired bullet we'll shoot
	int FirstBulletIndex = -1;	//used to find the bullet we're going to explode and re-shoot (only used if all bullets are active)
	//set an unfired bullet to active (check array for any bullets with ucShotOrder == 0)
	for (int i = 0; i < Bullets.Num(); i++)
	{
		//look for a bullet that hasn't been shot in the order yet
		if (Bullets[i]->GetShotOrder() == 0)
		{
			//increase active bullets (from 0 to 1 at start)
			ucActiveBullets++;
			//set the order this bullet was shot
			Bullets[i]->SetShotOrder(ucActiveBullets);
			//Save i to access the bullet later when shooting
			BulletIndex = i;
			//leave for loop, we are only shooting this bullet
			break;
		}

		//finding the first bullet in the order
		if (Bullets[i]->GetShotOrder() == 1)
		{
			FirstBulletIndex = i;
		}
	}

	//if we did shoot find an unfired bullet
	if (BulletIndex != -1)
	{
		//fire the bullet
		Bullets[BulletIndex]->Server_Fire(hit, dir, BurnDamage, BurnDuration, HitsPerSecond);
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "Fired bullet " + FString::FromInt(BulletIndex));
	}
	//ALL BULLETS HAVE BEEN SHOT
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "All shots have been fired");
		if (FirstBulletIndex != -1)
		{
			//explode the first bullet we shot (should be armed by the time all bullets are out)
			Bullets[FirstBulletIndex]->Server_Explode(ExplosionDamage);
			//reduce the counter on the other bullets
			for (int i = 0; i < Bullets.Num(); i++)
			{
				//take their current shot order, decrease by one, moving their explode position in the array
				Bullets[i]->SetShotOrder(Bullets[i]->GetShotOrder() - 1);
				//we don't need to do any checks because they've all been shot
			}
			//set this bullet's shot to our active amount
			Bullets[FirstBulletIndex]->SetShotOrder(ucActiveBullets);
			//fire the bullet again
			Bullets[FirstBulletIndex]->Server_Fire(hit, dir, BurnDamage, BurnDuration, HitsPerSecond);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "Could not find first bullet we shot");
		}
	}
}

void UEngineerBulletPoolComponent::Server_ExplodeBullets_Implementation(float ExplosionDamage)
{
	//loop through the queue calling explode on all active bullets
	for (int i = 0; i < Bullets.Num(); i++)
	{
		//if a bullet has been shot
		if (Bullets[i]->GetShotOrder() > 0)
		{
			//explode bullet
			Bullets[i]->Server_Explode(ExplosionDamage);
		}
	}
	//we have no active bullets anymore
	ucActiveBullets = 0;
}

TArray<FVector> UEngineerBulletPoolComponent::GetBulletLocations()
{
	TArray<FVector> locs;
	if (ucActiveBullets > 0)
	{

		//loop through the queue calling explode on all active bullets
		for (int i = 0; i < Bullets.Num(); i++)
		{
			//if a bullet has been shot
			if (Bullets[i]->GetShotOrder() > 0)
			{
				locs.Add(Bullets[i]->GetActorLocation());
			}
		}
	}
	return locs;
}

void UEngineerBulletPoolComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UEngineerBulletPoolComponent, Bullets);
}