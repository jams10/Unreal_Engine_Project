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

	// 스켈레탈 메시 컴포넌트 생성
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
	// 총알이 없는 경우
	if( LoadedBullet <= 0 )
	{
		UGameplayStatics::SpawnSoundAttached( OutOfAmmoSound, SkeletalMesh, TEXT( "MuzzleFlashSocket" ) );
		return;
	}

	LoadedBullet -= 1;
	if( LoadedBullet <= 0 ) LoadedBullet = 0;

	// 총구 화염 효과 재생
	if( MuzzleEffect )
	{
		UGameplayStatics::SpawnEmitterAttached( MuzzleEffect, SkeletalMesh, TEXT( "MuzzleFlashSocket" ) );
	}
	// 총알 발사 사운드 재생
	if( FireSound )
	{
		UGameplayStatics::SpawnSoundAttached( FireSound, SkeletalMesh, TEXT( "MuzzleFlashSocket" ) );
	}

	// 소유한 폰과 컨트롤러 가져오기
	if( OwnerPawn == nullptr )
	{
		//OwnerPawn = Cast<APawn>( GetOwner() );
		OwnerPawn = Cast<APawn>( GetAttachParentActor() );
		OwnerController = OwnerPawn->GetController();
	}

	// 총알 발사(라인 트레이스)
	if( OwnerController )
	{
		FVector Location;
		FRotator Rotation;

		// 카메라 뷰 포인트 얻어오기
		OwnerController->GetPlayerViewPoint( Location, Rotation );

		// 총알이 최대로 닿는 지점(라인 트레이스의 끝 지점)
		//	    뷰 포인트의 시작점 + 회전->방향 단위 벡터 * 총 사거리
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
				// 총알이 어디서 왔는지 알려주는 Vector. 총알이 온 방향의 반대를 취해주면 된다.
				FVector ShotDirection = -Rotation.Vector();

				// 총알이 충돌한 지점에 디버깅용 포인트 그리기
				//DrawDebugPoint( GetWorld(), Hit.Location, 10, FColor::Red, true );

				// 총알 충돌 효과 재생
				if( ImpactEffect )
				{
					UGameplayStatics::SpawnEmitterAtLocation( GetWorld(), ImpactEffect, Hit.Location, ShotDirection.Rotation() );
				}

				// 총알 충돌 사운드 재생
				if( ImpactSound )
				{
					UGameplayStatics::PlaySoundAtLocation( GetWorld(), ImpactSound, Hit.Location );
					MakeNoise( 1.0f, OwnerPawn, GetActorLocation() );
				}

				// 총을 맞은 액터를 얻어오기
				AActor* HitActor = Hit.GetActor();

				// 맞았을 때의 해당 엑터에 대한 작업 처리
				if( HitActor )
				{

					// 피격 물리 효과를 위해 EnemyCharacter의 ReactionByHit 함수 호출
					EnemyCharacter = Cast<AEnemyCharacter>( HitActor );

					if( EnemyCharacter )
					{
						// 히트 되었을 때 본 이름이 Head 일 경우 기존 데미지 * 5 만큼의 데미지 가함.
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
