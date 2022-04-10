// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickupItem.h"
#include "GunItem.generated.h"

UENUM( BlueprintType )
enum class EGunType : uint8
{
	GT_NOTHING UMETA( DisplayName = "Nothing" ),
	GT_PISTOL UMETA( DisplayName = "Pistol" ),
	GT_RIFLE UMETA( DisplayName = "Rifle" ),
	
	GT_MAX UMETA( DisplayName = "MAX" )
};

UCLASS()
class THE_CALL_API AGunItem : public APickupItem
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGunItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick( float DeltaTime ) override;

public:

	// ��� ó�� �Լ�
	void PullTrigger();

	// ������ ó�� �Լ�
	void Reload( int32& Ammo );

	// ���� Visibility Ȱ��ȭ 
	void SetWeaponVisibility( const bool& State );

	// ��� ������ Ÿ��
	float GetShotDelayTime() const;

	// ������ ������ Ÿ��
	float GetReloadDelayTime() const;

	// ���� Ÿ�� ����
	EGunType GetWeaponType() const;

	// ������ �Ѿ� ���� ����
	int32 GetLoadedBulletNumber() const;

	// �ִ� ���� �Ѿ� ���� ����
	int32 GetMaxLoadedBulletNumber() const;

	// ���� ���� ����(ĳ���Ϳ��� ���� ���� ���)
	USoundBase* GetReloadingSound() const;

	// ������ ���. ���⼭�� ���⸦ '����' �ϴ� ������ �����.
	void UseItem( class AShooterCharacter* PlayerCharacter ) override;

	/** ������ �̺�Ʈ ���ε� �Լ� */
	void OnOverlapBegin( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult ) override;

	void OnOverlapEnd( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex ) override;

public:

	UPROPERTY( VisibleAnywhere, Category = "Gun Mesh" )
	USkeletalMeshComponent* SkeletalMesh;

	/** ���� �ֿ����� ���� üũ */
	bool IsPickedUp;

	/** �̻�� �� ���� ���� ��ġ */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "EquipSocketName" )
	FName EquipSocketName;

	/* �� ������ ���� */
	UFUNCTION( BlueprintCallable )
	UTexture2D* GetGunIcon();

private:

	/* �� Property */
	UPROPERTY( EditAnywhere, Category = "Property" )
	EGunType GunType;

	UPROPERTY( EditAnywhere, Category = "Property" )
	float GunRange;

	UPROPERTY( EditAnywhere, Category = "Property" )
	float GunDamage;

	UPROPERTY( EditAnywhere, Category = "Property" )
	int32 LoadedBullet;

	UPROPERTY( EditAnywhere, Category = "Property" )
	int32 MaxBullet;

	UPROPERTY( EditAnywhere, Category = "Property" )
	int32 BulletsToShot;

	UPROPERTY( EditAnywhere, Category = "Property" )
	float BulletSpreadHalfAngleDegree;

	UPROPERTY( EditAnywhere, Category = "Property" )
	float ShotDelayTime;

	UPROPERTY( EditAnywhere, Category = "Property" )
	float ReloadDelayTime;

	/* �� ����Ʈ */
	UPROPERTY( EditAnywhere, Category = "Effects" )
	UParticleSystem* MuzzleEffect;

	UPROPERTY( EditAnywhere, Category = "Effects" )
	UParticleSystem* ImpactEffect;

	/* �� ���� */
	UPROPERTY( EditAnywhere, Category = "Sound" )
	USoundBase* OutOfAmmoSound;

	UPROPERTY( EditAnywhere, Category = "Sound" )
	USoundBase* FireSound;

	UPROPERTY( EditAnywhere, Category = "Sound" )
	USoundBase* ImpactSound;

	UPROPERTY( EditAnywhere, Category = "Sound" )
	USoundBase* ReloadingSound;
	
	/* �������� ����� �� ������ */
	UPROPERTY( EditAnywhere, Category = "GunIcon" )
	UTexture2D* GunIcon;

public:
	/* ĳ���� �ִϸ��̼� */
	UPROPERTY( EditAnywhere, Category = "UpperBodyMontage" )
	class UAnimMontage* FireAnimation;

	UPROPERTY( EditAnywhere, Category = "UpperBodyMontage" )
	class UAnimMontage* ReloadAnimation;

	UPROPERTY( EditAnywhere, Category = "UpperBodyMontage" )
	class UAnimMontage* SwapAnimation;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Attack" )
	TSubclassOf<UDamageType> DamageTypeClass;

private:

	class AEnemyCharacter* EnemyCharacter;

	/* ���� ������ ���� �ش� ���� ��Ʈ�ѷ� */
	APawn* OwnerPawn;
	AController* OwnerController;
};
