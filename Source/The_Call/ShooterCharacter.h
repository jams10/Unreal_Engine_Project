// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "GunItem.h"

#include "ShooterCharacter.generated.h"


UCLASS()
class THE_CALL_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// �Է� ���� �Լ���
	void MoveForward( float AxisValue );
	void MoveRight( float AxisValue );
	void AimOn();
	void AimOff();
	void FireOn();
	void FireOff();
	void SprintOn();
	void SprintOff();
	void Reload();
	void SwapWeapon( const EGunType GunTypeSwapFor );

	// ������ ���� �Լ�
	void Equip( class AGunItem* GunItem );

	void IncreaseHealth( const float Health );

	// ���� ���� �������Ʈ���� ȣ���� �� �ֵ��� �����ϴ� �Լ���
	UFUNCTION( BlueprintCallable, Category = "PlayerState" )
	bool GetIsAiming() const;

	UFUNCTION( BlueprintCallable, Category = "PlayerState" )
	bool GetIsFiring() const;

	UFUNCTION( BlueprintCallable, Category = "PlayerState" )
	bool GetIsPistol() const;

	UFUNCTION( BlueprintCallable, Category = "PlayerState" )
	bool IsDead() const;

	UFUNCTION( BlueprintCallable, Category = "PlayerItem" )
	void PickupWeapon( class AGunItem* GunPicked );

	UFUNCTION( BlueprintCallable, Category = "PlayerItem" )
	int32 GetAmmo() const;

	UFUNCTION( BlueprintCallable, Category = "PlayerItem" )
	int32 GetCurrentWeaponLoadedAmmo() const;

	UFUNCTION( BlueprintCallable, Category = "PlayerState" )
	float GetMaxHealth() const;

	UFUNCTION( BlueprintCallable, Category = "PlayerState" )
	float GetCurrentHealth() const;

	UFUNCTION( BlueprintCallable )
	AGunItem* GetCurrentHoldingGun();

	UFUNCTION( BlueprintCallable )
	void SetCurrentHoldingGun( AGunItem* Gun );

	virtual float TakeDamage( float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser ) override;

private:
	
	/* Spring Arm �� Camera ���� ���� */
	
	// Spring Arm �� Camera ���� ��
	UPROPERTY( EditAnywhere, Category = "Spring Arm and Camera" )
	float InitialSpringArmLength;

	UPROPERTY( EditAnywhere, Category = "Spring Arm and Camera" )
	float InitialCameraFOV;

	UPROPERTY( EditAnywhere, Category = "Spring Arm and Camera" )
	float SpringArmZoomAmount;

	UPROPERTY( EditAnywhere, Category = "Spring Arm and Camera" )
	float CameraZoomAmount;

	// Spring Arm �� Camera ������Ʈ �ν��Ͻ�
	UPROPERTY( EditAnywhere )
	class USpringArmComponent* SpringArm;

	UPROPERTY( EditAnywhere )
	class UCameraComponent* Camera;

	/* ĳ���� �̵� �ӵ� ���� ���� */
	UPROPERTY( EditAnywhere, Category = "CharacterMovement Speed" )
	float WalkSpeed;

	UPROPERTY( EditAnywhere, Category = "CharacterMovement Speed" )
	float RunSpeedIncrement;

	UPROPERTY( EditAnywhere, Category = "CharacterMovement Speed" )
	float SpeedWhenZoomIn;

	/* �� */
	UPROPERTY( EditDefaultsOnly )
	TArray<TSubclassOf<class AGun>> GunClass;

	UPROPERTY()
	TArray<class AGun*> Guns;

	UPROPERTY( EditDefaultsOnly )
	TArray<TSubclassOf<AGunItem>> GunItemClass;

	UPROPERTY()
	TArray<AGunItem*> GunItems;

	UPROPERTY( EditAnywhere )
	int MAX_WEAPON_NUMBER;

	/** ���� */
	UPROPERTY( EditAnywhere, Category = "Sound" )
	USoundBase* GettingHitVoiceSound; // �¾��� �� ����

	/* ĳ���� UpperBody �ִϸ��̼� */
	UPROPERTY( EditAnywhere, Category = "UpperBodyMontage")
	class UAnimMontage* PistolFireAnimation;

	UPROPERTY( EditAnywhere, Category = "UpperBodyMontage" )
	class UAnimMontage* PistolReloadAnimation;

	UPROPERTY( EditAnywhere, Category = "UpperBodyMontage" )
	class UAnimMontage* RifleFireAnimation;

	UPROPERTY( EditAnywhere, Category = "UpperBodyMontage" )
	class UAnimMontage* RifleReloadAnimation;

	UPROPERTY( EditAnywhere, Category = "UpperBodyMontage" )
	class UAnimMontage* SwapPistolAnimation;

	UPROPERTY( EditAnywhere, Category = "UpperBodyMontage" )
	class UAnimMontage* SwapRifleAnimation;

	UPROPERTY( EditAnywhere, Category = "UpperBodyMontage" )
	class UAnimMontage* HitAnimation;

private:

	float MaxHealth;
	float CurrentHealth;

	bool IsAiming;
	bool IsFiring;
	bool canFire;
	bool IsReloading;
	bool IsSwapping;
	bool IsPistol;
	float ElapsedTime;
	int32 CurrentWeaponIndex;

	int32 PistolAmmo;
	int32 ShotGunAmmo;

	FTimerHandle ReloadDelayTimerHandle;
	FTimerHandle WeaponSwapDelayTimerHandle;
	FTimerHandle ShotDelayTimerHandle;

	AGunItem* CurrentHoldingGun;

	TArray<AGunItem> GunsInInventory;

	class AShooterPlayerController* PlayerController;
public:

	UPROPERTY( VisibleAnywhere, BlueprintReadWrite = "Toogle Inventory" );
	bool bInventoryIsOpen;

	/** �÷��� */
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	class USpotLightComponent* FlashLight;

private:

	bool IsValidAmmoInInvertory( EGunType GunType ) const;

	void ResetReloadTimer();
	void ResetSwapTimer();
	void ResetShotDelayTimer();
};
