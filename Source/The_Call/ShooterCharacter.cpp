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

	// ������ �ϰ� ī�޶� ���� �ʱ� ��
	InitialSpringArmLength = 150.0f;
	InitialCameraFOV = 90.0f;
	SpringArmZoomAmount = 50.0f;
	CameraZoomAmount = 5.0f;

	// ������ �� ������Ʈ ����
	SpringArm = CreateDefaultSubobject<USpringArmComponent>( TEXT( "SpringArm" ) );
	SpringArm->SetupAttachment( RootComponent );
	SpringArm->TargetArmLength = InitialSpringArmLength;
	SpringArm->bUsePawnControlRotation = true;

	// ī�޶� ������Ʈ ����
	Camera = CreateDefaultSubobject<UCameraComponent>( TEXT( "Camera" ) );
	Camera->SetupAttachment( SpringArm, USpringArmComponent::SocketName );
	Camera->FieldOfView = InitialCameraFOV;
	Camera->bUsePawnControlRotation = false;

	// ����Ʈ ����Ʈ ������Ʈ ����
	FlashLight = CreateDefaultSubobject<USpotLightComponent>( TEXT( "FlashLight" ) );
	FlashLight->SetupAttachment( GetMesh(), FName("FlashSocket") );
	FlashLight->SetVisibility( false );

	// ��Ʈ�ѷ� ȸ�� �� ĳ���� �̵� ȸ�� ����(���� �ÿ��� ��Ʈ�ѷ��� Yaw ���� ���� �� �ֵ��� �ʱⰪ�� ��� false)
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;

	// ĳ���� �̵� �ӵ� �ʱ� �� ����
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

	// ���� �� ��/�ƿ��� ���� ���ŵ� ���ο� ����
	float NewSpringArmLength = InitialSpringArmLength;
	float NewCameraFov = InitialCameraFOV;

	// ������ �� ���̿� ī�޶� �þ� ���� �ε巴�� ��ȯ�ǵ��� ������ ���.
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

	// ĳ���� �̵��ÿ��� ĳ������ ȸ���� ��Ʈ�ѷ��� ����� ��ġ��Ŵ.
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
			// ������ �Ѿ��� 0�� ��� PullTrigger���� �Ѿ� ���� ���带 ����ϰ� �ٷ� ����.
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
	// ���� ���°� �ƴ� ���� ������Ʈ ����
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
		// �ѿ� �̹� �ִ� ������ �Ѿ��� �����Ǿ� ������ �׳� ����.
		if( CurrentHoldingGun->GetLoadedBulletNumber() == CurrentHoldingGun->GetMaxLoadedBulletNumber() ) return;
		// �κ��丮�� ���� ����ִ� ���� �Ѿ��� �־�߸� ���� �۾� ����.
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
			// �κ��丮���� ���� ����̵Ǵ� ���Ⱑ �ִ��� Ž��
			for( auto Item : PlayerController->Inventory )
			{
				AGunItem* GunForSwap = Cast<AGunItem>( Item );

				// �ٲ� ���Ⱑ �����ϸ�
				if( GunForSwap && GunForSwap->GetWeaponType() == GunTypeSwapFor )
				{
					IsSwapping = true;

					// ������ ��� �ִ� ���⸦ �ٸ� ���Ͽ� ����
					const USkeletalMeshSocket* EquipSocket = GetMesh()->GetSocketByName( CurrentHoldingGun->EquipSocketName );

					if( EquipSocket )
					{
						EquipSocket->AttachActor( CurrentHoldingGun, GetMesh() );
					}

					// ���� �ִϸ��̼� ���
					if( CurrentHoldingGun->SwapAnimation )
					{
						PlayAnimMontage( CurrentHoldingGun->SwapAnimation, 1.5f );
					}

					// ��� �ִ� ���� �����ϰ� �ش� ����� �տ� ������.
					CurrentHoldingGun = GunForSwap;

					const USkeletalMeshSocket* WeaponSocket = GetMesh()->GetSocketByName( "WeaponSocket" );
					if( WeaponSocket )
					{
						WeaponSocket->AttachActor( CurrentHoldingGun, GetMesh() );
					}

					// ���� ������
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
