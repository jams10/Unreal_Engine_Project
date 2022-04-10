// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"

//#include "GunItem.h"
#include "ShooterPlayerController.h"
#include "AmmoItem.h"

#include "Animation/AnimMontage.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SpotLightComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "TimerManager.h"
#include "Animation/AnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Sound/SoundBase.h"

// Sets default values
AShooterCharacter::AShooterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 스프링 암과 카메라 설정 초기 값
	InitialSpringArmLength = 150.0f;
	InitialCameraFOV = 90.0f;
	SpringArmZoomAmount = 50.0f;
	CameraZoomAmount = 5.0f;

	// 스프링 암 컴포넌트 생성
	SpringArm = CreateDefaultSubobject<USpringArmComponent>( TEXT( "SpringArm" ) );
	SpringArm->SetupAttachment( RootComponent );
	SpringArm->TargetArmLength = InitialSpringArmLength;
	SpringArm->bUsePawnControlRotation = true;

	// 카메라 컴포넌트 생성
	Camera = CreateDefaultSubobject<UCameraComponent>( TEXT( "Camera" ) );
	Camera->SetupAttachment( SpringArm, USpringArmComponent::SocketName );
	Camera->FieldOfView = InitialCameraFOV;
	Camera->bUsePawnControlRotation = false;

	// 포인트 라이트 컴포넌트 생성
	FlashLight = CreateDefaultSubobject<USpotLightComponent>( TEXT( "FlashLight" ) );
	FlashLight->SetupAttachment( GetMesh(), FName("FlashSocket") );
	FlashLight->SetVisibility( false );

	// 컨트롤러 회전 및 캐릭터 이동 회전 설정(조준 시에만 컨트롤러의 Yaw 값을 따라갈 수 있도록 초기값은 모두 false)
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;

	// 캐릭터 이동 속도 초기 값 설정
	WalkSpeed = 200.0f;
	RunSpeedIncrement = 250.0f;
	SpeedWhenZoomIn = 125.0f;

	this->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	MaxHealth = 100.f;
	CurrentHealth = 60.f;

	IsAiming = false;
	IsFiring = false;
	canFire = true;
	IsReloading = false;
	IsSwapping = false;
	IsPistol = true;

	MAX_WEAPON_NUMBER = 2;
	CurrentWeaponIndex = 0;

	PistolAmmo = 30;
	ShotGunAmmo = 24;

	bInventoryIsOpen = false;
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	ElapsedTime = GetWorld()->GetTimeSeconds();

	PlayerController = Cast<AShooterPlayerController>( UGameplayStatics::GetPlayerController( GetWorld(), 0 ) );
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 에임 줌 인/아웃에 따라 갱신될 새로운 값들
	float NewSpringArmLength = InitialSpringArmLength;
	float NewCameraFov = InitialCameraFOV;

	// 스프링 암 길이와 카메라 시야 값은 부드럽게 전환되도록 보간을 사용.
	if( IsAiming )
	{
		NewSpringArmLength =
			FMath::FInterpTo(
				SpringArm->TargetArmLength,
				InitialSpringArmLength - SpringArmZoomAmount,
				DeltaTime,
				10.0f );

		NewCameraFov =
			FMath::FInterpTo(
				Camera->FieldOfView,
				InitialCameraFOV - CameraZoomAmount,
				DeltaTime,
				10.0f );

		bUseControllerRotationYaw = true;
	}
	else
	{
		NewSpringArmLength =
			FMath::FInterpTo(
				SpringArm->TargetArmLength,
				InitialSpringArmLength,
				DeltaTime,
				10.0f );

		NewCameraFov =
			FMath::FInterpTo(
				Camera->FieldOfView,
				InitialCameraFOV,
				DeltaTime,
				10.0f );

		bUseControllerRotationYaw = false;
	}

	SpringArm->TargetArmLength = NewSpringArmLength;
	Camera->FieldOfView = NewCameraFov;

	// 캐릭터 이동시에만 캐릭터의 회전이 컨트롤러의 방향과 일치시킴.
	if( GetVelocity().Size() > 0 )
	{
		FVector Location;
		FRotator Rotation;

		GetController()->GetPlayerViewPoint( Location, Rotation );

		FRotator NewRotation =
			FMath::RInterpTo(
				GetActorRotation(),
				Rotation,
				DeltaTime,
				10.0f );

		FRotator ActorRotation = GetActorRotation();

		SetActorRotation( FRotator( ActorRotation.Pitch, NewRotation.Yaw, ActorRotation.Roll ) );
	}
}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis( TEXT( "MoveForward" ), this, &AShooterCharacter::MoveForward );
	PlayerInputComponent->BindAxis( TEXT( "MoveRight" ), this, &AShooterCharacter::MoveRight );
	PlayerInputComponent->BindAxis( TEXT( "LookUp" ), this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis( TEXT( "Turn" ), this, &APawn::AddControllerYawInput );

	PlayerInputComponent->BindAction( TEXT( "Aim" ), IE_Pressed, this, &AShooterCharacter::AimOn );
	PlayerInputComponent->BindAction( TEXT( "Aim" ), IE_Released, this, &AShooterCharacter::AimOff );

	PlayerInputComponent->BindAction( TEXT( "Fire" ), IE_Pressed, this, &AShooterCharacter::FireOn );
	PlayerInputComponent->BindAction( TEXT( "Fire" ), IE_Released, this, &AShooterCharacter::FireOff );

	PlayerInputComponent->BindAction( TEXT( "Sprint" ), IE_Pressed, this, &AShooterCharacter::SprintOn );
	PlayerInputComponent->BindAction( TEXT( "Sprint" ), IE_Released, this, &AShooterCharacter::SprintOff );

	PlayerInputComponent->BindAction( TEXT( "Reload" ), IE_Released, this, &AShooterCharacter::Reload );
	
	DECLARE_DELEGATE_OneParam( FCustomInputDelegate, const EGunType );
	PlayerInputComponent->BindAction<FCustomInputDelegate>( TEXT( "SwapWeaponIndexOne" ), IE_Pressed, this, &AShooterCharacter::SwapWeapon, EGunType::GT_PISTOL );

	DECLARE_DELEGATE_OneParam( FCustomInputDelegate, const EGunType );
	PlayerInputComponent->BindAction<FCustomInputDelegate>( TEXT( "SwapWeaponIndexTwo" ), IE_Pressed, this, &AShooterCharacter::SwapWeapon, EGunType::GT_RIFLE );
}

void AShooterCharacter::MoveForward( float AxisValue )
{
	if( bInventoryIsOpen ) return;
	AddMovementInput( GetActorForwardVector(), AxisValue );
}

void AShooterCharacter::MoveRight( float AxisValue )
{
	if( bInventoryIsOpen ) return;
	AddMovementInput( GetActorRightVector(), AxisValue );
}

void AShooterCharacter::AimOn()
{
	if( bInventoryIsOpen ) return;
	IsAiming = true;
	this->GetCharacterMovement()->MaxWalkSpeed = SpeedWhenZoomIn;
}

void AShooterCharacter::AimOff()
{
	if( bInventoryIsOpen ) return;
	IsAiming = false;
	this->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AShooterCharacter::FireOn()
{
	if( bInventoryIsOpen ) return;
	if( CurrentHoldingGun )
	{
		if( CurrentHoldingGun->GetLoadedBulletNumber() <= 0 )
		{
			// 장전된 총알이 0인 경우 PullTrigger에서 총알 없음 사운드를 출력하고 바로 리턴.
			CurrentHoldingGun->PullTrigger();
			return;
		}
		if( IsAiming )
		{
			if( canFire )
			{
				canFire = false;
				IsFiring = true;

				PlayAnimMontage( CurrentHoldingGun->FireAnimation, 1.f );
				GetWorldTimerManager().SetTimer( ShotDelayTimerHandle, this, &AShooterCharacter::ResetShotDelayTimer, CurrentHoldingGun->GetShotDelayTime() );
				CurrentHoldingGun->PullTrigger();
			}
		}
	}
}

void AShooterCharacter::FireOff()
{
	IsFiring = false;
}

void AShooterCharacter::SprintOn()
{
	if( bInventoryIsOpen ) return;
	// 조준 상태가 아닐 때만 스프린트 가능
	if( !IsAiming )
	{
		this->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed + RunSpeedIncrement;
	}
}

void AShooterCharacter::SprintOff()
{
	if( IsAiming )
	{
		this->GetCharacterMovement()->MaxWalkSpeed = SpeedWhenZoomIn;
	}
	else this->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AShooterCharacter::Reload()
{
	if( bInventoryIsOpen ) return;
	if( CurrentHoldingGun && !IsReloading )
	{
		// 총에 이미 최대 개수의 총알이 장전되어 있으면 그냥 리턴.
		if( CurrentHoldingGun->GetLoadedBulletNumber() == CurrentHoldingGun->GetMaxLoadedBulletNumber() ) return;
		// 인벤토리에 지금 들고있는 총의 총알이 있어야만 장전 작업 수행.
		if( IsValidAmmoInInvertory( CurrentHoldingGun->GetWeaponType() ) )
		{
			IsReloading = true;

			PlayAnimMontage( CurrentHoldingGun->ReloadAnimation, 1.f );

			UGameplayStatics::PlaySoundAtLocation( GetWorld(), CurrentHoldingGun->GetReloadingSound(), GetActorLocation() );

			GetWorldTimerManager().SetTimer( ReloadDelayTimerHandle, this, &AShooterCharacter::ResetReloadTimer, CurrentHoldingGun->GetReloadDelayTime() );
		}
	}
}

void AShooterCharacter::SwapWeapon( const EGunType GunTypeSwapFor )
{
	if( CurrentHoldingGun )
	{
		if( CurrentHoldingGun->GetWeaponType() == GunTypeSwapFor ) return;

		if( !IsSwapping )
		{
			// 인벤토리에서 스왑 대상이되는 무기가 있는지 탐색
			for( auto Item : PlayerController->Inventory )
			{
				AGunItem* GunForSwap = Cast<AGunItem>( Item );

				// 바꿀 무기가 존재하면
				if( GunForSwap && GunForSwap->GetWeaponType() == GunTypeSwapFor )
				{
					IsSwapping = true;

					// 기존에 들고 있는 무기를 다른 소켓에 부착
					const USkeletalMeshSocket* EquipSocket = GetMesh()->GetSocketByName( CurrentHoldingGun->EquipSocketName );

					if( EquipSocket )
					{
						EquipSocket->AttachActor( CurrentHoldingGun, GetMesh() );
					}

					// 스왑 애니메이션 재생
					if( CurrentHoldingGun->SwapAnimation )
					{
						PlayAnimMontage( CurrentHoldingGun->SwapAnimation, 1.5f );
					}

					// 들고 있는 무기 변경하고 해당 무기는 손에 부착함.
					CurrentHoldingGun = GunForSwap;

					const USkeletalMeshSocket* WeaponSocket = GetMesh()->GetSocketByName( "WeaponSocket" );
					if( WeaponSocket )
					{
						WeaponSocket->AttachActor( CurrentHoldingGun, GetMesh() );
					}

					// 스왑 딜레이
					GetWorldTimerManager().SetTimer( WeaponSwapDelayTimerHandle, this, &AShooterCharacter::ResetSwapTimer, 1.5f );
					break;
				}
			}
		}
	}
}

void AShooterCharacter::Equip( AGunItem* GunItem )
{
	if( CurrentHoldingGun != GunItem )
	{
		SwapWeapon( GunItem->GetWeaponType() );
	}
}

void AShooterCharacter::IncreaseHealth( const float Health )
{
	CurrentHealth = FMath::Min( CurrentHealth + Health, MaxHealth );
}

bool AShooterCharacter::GetIsAiming() const
{
	return IsAiming;
}

bool AShooterCharacter::GetIsFiring() const
{
	return IsFiring;
}

bool AShooterCharacter::GetIsPistol() const
{
	if( CurrentHoldingGun )
	{
		return CurrentHoldingGun->GetWeaponType() == EGunType::GT_PISTOL;
	}
	else return false;
}

bool AShooterCharacter::IsDead() const
{
	return CurrentHealth <= 0.f ;
}

void AShooterCharacter::PickupWeapon( AGunItem* GunPicked )
{
	for( int i = 0; i < MAX_WEAPON_NUMBER; ++i )
	{
		if( GunPicked->ItemName.ToString() == GunItems[CurrentWeaponIndex]->ItemName.ToString() )
		{
			GunItems[i]->IsPickedUp = true;
			GunItems[i]->SetWeaponVisibility( true );
			break;
		}
	}
}

int32 AShooterCharacter::GetAmmo() const
{
	int32 WholeAmmo = 0;

	if( CurrentHoldingGun )
	{
		EGunType CurrentHoldingGunType = CurrentHoldingGun->GetWeaponType();

		for( auto Item : PlayerController->Inventory )
		{
			AAmmoItem* Ammo = Cast<AAmmoItem>( Item );

			if( Ammo )
			{
				if( CurrentHoldingGun->GetWeaponType() == EGunType::GT_PISTOL && Ammo->AmmoType == EAmmoType::AT_PISTOL )
				{
					WholeAmmo += Ammo->StackAmount;
				}
				else if( CurrentHoldingGun->GetWeaponType() == EGunType::GT_RIFLE && Ammo->AmmoType == EAmmoType::AT_RIFLE )
				{
					WholeAmmo += Ammo->StackAmount;
				}
			}
		}
	}
	return WholeAmmo;
}

int32 AShooterCharacter::GetCurrentWeaponLoadedAmmo() const
{
	if( CurrentHoldingGun )
	{
		return CurrentHoldingGun->GetLoadedBulletNumber();
	}
	else return 0;
}

float AShooterCharacter::GetMaxHealth() const
{
	return MaxHealth;
}

float AShooterCharacter::GetCurrentHealth() const
{
	return CurrentHealth;
}

AGunItem* AShooterCharacter::GetCurrentHoldingGun()
{
	return CurrentHoldingGun;
}

void AShooterCharacter::SetCurrentHoldingGun( AGunItem* Gun )
{
	CurrentHoldingGun = Gun;

	const USkeletalMeshSocket* WeaponSocket = GetMesh()->GetSocketByName( "WeaponSocket" );
	if( WeaponSocket )
	{
		WeaponSocket->AttachActor( CurrentHoldingGun, GetMesh() );
	}
}

float AShooterCharacter::TakeDamage( float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser )
{
	{
		float DamageToApply = Super::TakeDamage( DamageAmount, DamageEvent, EventInstigator, DamageCauser );

		DamageToApply = FMath::Min( CurrentHealth, DamageToApply );

		CurrentHealth -= DamageToApply;

		UE_LOG( LogTemp, Warning, TEXT( "NAME : %s / HEALTH : %f" ), *GetName(), CurrentHealth );

		UGameplayStatics::PlaySoundAtLocation( GetWorld(), GettingHitVoiceSound, GetActorLocation() );

		PlayAnimMontage( HitAnimation, 1.5f );

		return DamageAmount;
	}
}

bool AShooterCharacter::IsValidAmmoInInvertory( EGunType GunType ) const
{
	for( auto Item : PlayerController->Inventory )
	{
		AAmmoItem* Ammo = Cast<AAmmoItem>( Item );

		if( Ammo )
		{
			if( GunType == EGunType::GT_PISTOL && Ammo->AmmoType == EAmmoType::AT_PISTOL )
			{
				return true;
			}
			else if( GunType == EGunType::GT_RIFLE && Ammo->AmmoType == EAmmoType::AT_RIFLE )
			{
				return true;
			}
		}
	}
	return false;
}

void AShooterCharacter::ResetReloadTimer()
{
	IsReloading = false;

	if( CurrentHoldingGun )
	{
		int32 index = 0;

		while( index < PlayerController->Inventory.Num() )
		{
			AAmmoItem* Ammo = Cast<AAmmoItem>( PlayerController->Inventory[index] );
			
			if( Ammo )
			{			
				if( CurrentHoldingGun->GetWeaponType() == EGunType::GT_PISTOL && Ammo->AmmoType == EAmmoType::AT_PISTOL )
				{
					CurrentHoldingGun->Reload( Ammo->StackAmount );
					if( Ammo->StackAmount == 0 )
					{
						PlayerController->Inventory.Remove( Ammo );
						PlayerController->RefreshInventory();
						index--;
					}
				}
				else if( CurrentHoldingGun->GetWeaponType() == EGunType::GT_RIFLE && Ammo->AmmoType == EAmmoType::AT_RIFLE )
				{
					CurrentHoldingGun->Reload( Ammo->StackAmount );
					if( Ammo->StackAmount == 0 )
					{
						PlayerController->Inventory.Remove( Ammo );
						PlayerController->RefreshInventory();
						index--;
					}
				}
			}
			index++;
		}
	}
}

void AShooterCharacter::ResetSwapTimer()
{
	IsSwapping = false;
}

void AShooterCharacter::ResetShotDelayTimer()
{
	canFire = true;
}
