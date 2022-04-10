// Fill out your copyright notice in the Description page of Project Settings.


#include "GunItem.h"

#include "ShooterCharacter.h"
#include "EnemyCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/PrimitiveComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystem.h"

AGunItem::AGunItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// ���̷�Ż �޽� ������Ʈ ����
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>( TEXT( "Skeletal Mesh" ) );
	SkeletalMesh->SetupAttachment( Root );

	ItemType = EItemType::IT_WEAPON;
	GunType = EGunType::GT_NOTHING;

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

void AGunItem::BeginPlay()
{
	Super::BeginPlay();

	CollisionVolume->OnComponentBeginOverlap.AddDynamic( this, &AGunItem::OnOverlapBegin );
	CollisionVolume->OnComponentEndOverlap.AddDynamic( this, &AGunItem::OnOverlapEnd );
}

void AGunItem::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

void AGunItem::PullTrigger()
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
		//OwnerPawn = Cast<APawn>( GetOwner() );
		OwnerPawn = Cast<APawn>( GetAttachParentActor() );
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
				//DrawDebugPoint( GetWorld(), Hit.Location, 10, FColor::Red, true );

				// �Ѿ� �浹 ȿ�� ���
				if( ImpactEffect )
				{
					UGameplayStatics::SpawnEmitterAtLocation( GetWorld(), ImpactEffect, Hit.Location, ShotDirection.Rotation() );
				}

				// �Ѿ� �浹 ���� ���
				if( ImpactSound )
				{
					UGameplayStatics::PlaySoundAtLocation( GetWorld(), ImpactSound, Hit.Location );
					MakeNoise( 1.0f, OwnerPawn, GetActorLocation() );
				}

				// ���� ���� ���͸� ������
				AActor* HitActor = Hit.GetActor();

				// �¾��� ���� �ش� ���Ϳ� ���� �۾� ó��
				if( HitActor )
				{

					// �ǰ� ���� ȿ���� ���� EnemyCharacter�� ReactionByHit �Լ� ȣ��
					EnemyCharacter = Cast<AEnemyCharacter>( HitActor );

					if( EnemyCharacter )
					{
						// ��Ʈ �Ǿ��� �� �� �̸��� Head �� ��� ���� ������ * 5 ��ŭ�� ������ ����.
						if( Hit.BoneName == "Head" )
						{
							UGameplayStatics::ApplyDamage(
								EnemyCharacter,
								GunDamage * 5,
								UGameplayStatics::GetPlayerController( GetWorld(), 0 ),
								UGameplayStatics::GetPlayerCharacter( GetWorld(), 0 ),
								DamageTypeClass );
						}
						else
						{
							UGameplayStatics::ApplyDamage(
								EnemyCharacter,
								GunDamage,
								UGameplayStatics::GetPlayerController( GetWorld(), 0 ),
								UGameplayStatics::GetPlayerCharacter( GetWorld(), 0 ),
								DamageTypeClass );
						}

						UGameplayStatics::SpawnEmitterAtLocation( GetWorld(), EnemyCharacter->HitParticle, Hit.Location, ShotDirection.Rotation() );
						EnemyCharacter->ReactionByHit( Hit.BoneName, Hit.Location );
					}
				}
			}
		}
	}
}

void AGunItem::Reload( int32& Ammo )
{
	if( Ammo <= 0 ) return;

	for( LoadedBullet; LoadedBullet < MaxBullet; ++LoadedBullet )
	{
		if( Ammo <= 0 ) return;
		Ammo--;
	}
}

void AGunItem::SetWeaponVisibility( const bool& State )
{
	SkeletalMesh->SetVisibility( State );
}

float AGunItem::GetShotDelayTime() const
{
	return ShotDelayTime;
}

float AGunItem::GetReloadDelayTime() const
{
	return ReloadDelayTime;
}

EGunType AGunItem::GetWeaponType() const
{
	return GunType;
}

int32 AGunItem::GetLoadedBulletNumber() const
{
	return LoadedBullet;
}

int32 AGunItem::GetMaxLoadedBulletNumber() const
{
	return MaxBullet;
}

USoundBase* AGunItem::GetReloadingSound() const
{
	return ReloadingSound;
}

void AGunItem::UseItem( AShooterCharacter* PlayerCharacter )
{
	PlayerCharacter->Equip( this );
}

void AGunItem::OnOverlapBegin( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult )
{
	Super::OnOverlapBegin( OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult );

	UE_LOG( LogTemp, Warning, TEXT( "GunItem OnOverlapBegin()" ) );
}

void AGunItem::OnOverlapEnd( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex )
{
	Super::OnOverlapEnd( OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex );

	UE_LOG( LogTemp, Warning, TEXT( "GunItem OnOverlapEnd()" ) );
}

UTexture2D* AGunItem::GetGunIcon()
{
	return GunIcon;
}
