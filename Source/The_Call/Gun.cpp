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

	// Root가 될 SceneComponent 생성
	Root = CreateDefaultSubobject<USceneComponent>( TEXT( "Root" ) );
	SetRootComponent( Root );

	// 스켈레탈 메시 컴포넌트 생성
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>( TEXT( "Skeletal Mesh" ) );
	SkeletalMesh->SetupAttachment( Root );
	
	// 콜리젼 볼륨 역할을 할 SphereComponent 생성.
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
		OwnerPawn = Cast<APawn>( GetOwner() );
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
				DrawDebugPoint( GetWorld(), Hit.Location, 10, FColor::Red, true );

				// 총알 충돌 효과 재생
				//if( ImpactEffect )
				//{
					//UGameplayStatics::SpawnEmitterAtLocation( GetWorld(), ImpactEffect, Hit.Location, ShotDirection.Rotation() );
				//}

				// 총을 맞은 액터를 얻어오기
				AActor* HitActor = Hit.GetActor();
	
				// 맞았을 때의 해당 엑터에 대한 작업 처리
				if( HitActor )
				{
					FPointDamageEvent DamageEvent( GunDamage, Hit, ShotDirection, nullptr );
					if( Hit.BoneName == "Head" )
					{
						HitActor->TakeDamage( GunDamage * 5, DamageEvent, OwnerController, this );
					}
					else HitActor->TakeDamage( GunDamage, DamageEvent, OwnerController, this );

					// 피격 물리 효과를 위해 EnemyCharacter의 ReactionByHit 함수 호출
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


