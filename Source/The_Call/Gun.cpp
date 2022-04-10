// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"

#include "ShooterCharacter.h"
#include "EnemyCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/SceneComponent.h"
#include "Components/PrimitiveComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystem.h"

// Sets default values
AGun::AGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Root�� �� SceneComponent ����
	Root = CreateDefaultSubobject<USceneComponent>( TEXT( "Root" ) );
	SetRootComponent( Root );

	// ���̷�Ż �޽� ������Ʈ ����
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>( TEXT( "Skeletal Mesh" ) );
	SkeletalMesh->SetupAttachment( Root );
	
	// �ݸ��� ���� ������ �� SphereComponent ����.
	CollisionVolume = CreateDefaultSubobject<USphereComponent>( TEXT( "CollisionVolume" ) );
	CollisionVolume->SetupAttachment( SkeletalMesh );

	WeaponType = EWeaponType::WT_NOTHING;
	GunRange = 1500.0f;
	GunDamage = 10.0f;
	LoadedBullet = 15;
	MaxBullet = 15;
	BulletsToShot = 1;
	BulletSpreadHalfAngleDegree = 1.f;
	ShotDelayTime = .2f;
	ReloadDelayTime = 1.5f;
	IsPickedUp = false;
}

void AGun::PullTrigger()
{
	// �Ѿ��� ���� ���
	if( LoadedBullet <= 0 )
	{
		UGameplayStatics::SpawnSoundAttached( OutOfAmmoSound, SkeletalMesh, TEXT( "MuzzleFlashSocket" ) );
		return;
	}

	LoadedBullet -= 1;
	if( LoadedBullet <= 0 ) LoadedBullet = 0;

	// �ѱ� ȭ�� ȿ�� ���
	if( MuzzleEffect )
	{
		UGameplayStatics::SpawnEmitterAttached( MuzzleEffect, SkeletalMesh, TEXT( "MuzzleFlashSocket" ) );
	}
	// �Ѿ� �߻� ���� ���
	if( FireSound )
	{
		UGameplayStatics::SpawnSoundAttached( FireSound, SkeletalMesh, TEXT( "MuzzleFlashSocket" ) );
	}

	// ������ ���� ��Ʈ�ѷ� ��������
	if( OwnerPawn == nullptr )
	{
		OwnerPawn = Cast<APawn>( GetOwner() );
		OwnerController = OwnerPawn->GetController();
	}

	// �Ѿ� �߻�(���� Ʈ���̽�)
	if( OwnerController )
	{
		FVector Location;
		FRotator Rotation;

		// ī�޶� �� ����Ʈ ������
		OwnerController->GetPlayerViewPoint( Location, Rotation );

		// �Ѿ��� �ִ�� ��� ����(���� Ʈ���̽��� �� ����)
		//	    �� ����Ʈ�� ������ + ȸ��->���� ���� ���� * �� ��Ÿ�
		//FVector End = Location + Rotation.Vector() * GunRange;

		for( int i = 0; i < BulletsToShot; ++i )
		{
			FVector SocketLocation = SkeletalMesh->GetSocketLocation( "MuzzleFlashSocket" );
			FVector Spread = UKismetMathLibrary::RandomUnitVectorInConeInDegrees( Rotation.Vector(), BulletSpreadHalfAngleDegree );
			FVector End = Location + Spread * GunRange;

			FHitResult Hit;
			bool bSuccess = GetWorld()->LineTraceSingleByChannel( Hit, Location, End, ECollisionChannel::ECC_GameTraceChannel1 );

			if( bSuccess )
			{
				// �Ѿ��� ��� �Դ��� �˷��ִ� Vector. �Ѿ��� �� ������ �ݴ븦 �����ָ� �ȴ�.
				FVector ShotDirection = -Rotation.Vector();

				// �Ѿ��� �浹�� ������ ������ ����Ʈ �׸���
				DrawDebugPoint( GetWorld(), Hit.Location, 10, FColor::Red, true );

				// �Ѿ� �浹 ȿ�� ���
				//if( ImpactEffect )
				//{
					//UGameplayStatics::SpawnEmitterAtLocation( GetWorld(), ImpactEffect, Hit.Location, ShotDirection.Rotation() );
				//}

				// ���� ���� ���͸� ������
				AActor* HitActor = Hit.GetActor();
	
				// �¾��� ���� �ش� ���Ϳ� ���� �۾� ó��
				if( HitActor )
				{
					FPointDamageEvent DamageEvent( GunDamage, Hit, ShotDirection, nullptr );
					if( Hit.BoneName == "Head" )
					{
						HitActor->TakeDamage( GunDamage * 5, DamageEvent, OwnerController, this );
					}
					else HitActor->TakeDamage( GunDamage, DamageEvent, OwnerController, this );

					// �ǰ� ���� ȿ���� ���� EnemyCharacter�� ReactionByHit �Լ� ȣ��
					EnemyCharacter = Cast<AEnemyCharacter>( HitActor );
					
					if( EnemyCharacter )
					{
						UGameplayStatics::SpawnEmitterAtLocation( GetWorld(), EnemyCharacter->HitParticle, Hit.Location, ShotDirection.Rotation() );
						EnemyCharacter->ReactionByHit( Hit.BoneName, Hit.Location );
					}
				}
			}
		}
	}

}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();
	
	CollisionVolume->OnComponentBeginOverlap.AddDynamic( this, &AGun::OnOverlapBegin );
	CollisionVolume->OnComponentEndOverlap.AddDynamic( this, &AGun::OnOverlapEnd );
}

// Called every frame
void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGun::SetWeaponVisibility( const bool& State )
{
	SkeletalMesh->SetVisibility( State );
}

float AGun::GetShotDelayTime() const
{
	return ShotDelayTime;
}

float AGun::GetReloadDelayTime() const
{
	return ReloadDelayTime;
}

EWeaponType AGun::GetWeaponType() const
{
	return WeaponType;
}

int32 AGun::GetLoadedBulletNumber() const
{
	return LoadedBullet;
}

void AGun::Reload( int32& Ammo )
{
	if( Ammo <= 0 ) return;

	for( LoadedBullet; LoadedBullet < MaxBullet; ++LoadedBullet )
	{
		if( Ammo <= 0 ) return;
		Ammo--;
	}
}

void AGun::OnOverlapBegin( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult )
{
	UE_LOG( LogTemp, Warning, TEXT( "OnOverlapBegin()" ) );
	
	if( CollisionVolume )
	{
		if( OtherActor )
		{
			AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>( OtherActor );
			if( ShooterCharacter )
			{
				//ShooterCharacter->PickupWeapon( this );
				Destroy();
			}
		}
	}
}

void AGun::OnOverlapEnd( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex )
{
	UE_LOG( LogTemp, Warning, TEXT( "OnOverlapEnd()" ) );
}

void AGun::ResetPhysicsTimer()
{
	EnemyCharacter->GetMesh()->SetSimulatePhysics( false );
}


