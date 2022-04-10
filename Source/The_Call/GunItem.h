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

	// 사격 처리 함수
	void PullTrigger();

	// 재장전 처리 함수
	void Reload( int32& Ammo );

	// 무기 Visibility 활성화 
	void SetWeaponVisibility( const bool& State );

	// 사격 딜레이 타임
	float GetShotDelayTime() const;

	// 재장전 딜레이 타임
	float GetReloadDelayTime() const;

	// 무기 타입 리턴
	EGunType GetWeaponType() const;

	// 장전된 총알 개수 리턴
	int32 GetLoadedBulletNumber() const;

	// 최대 장전 총알 개수 리턴
	int32 GetMaxLoadedBulletNumber() const;

	// 장전 사운드 리턴(캐릭터에서 장전 사운드 재생)
	USoundBase* GetReloadingSound() const;

	// 아이템 사용. 여기서는 무기를 '장착' 하는 것으로 사용함.
	void UseItem( class AShooterCharacter* PlayerCharacter ) override;

	/** 오버랩 이벤트 바인딩 함수 */
	void OnOverlapBegin( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult ) override;

	void OnOverlapEnd( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex ) override;

public:

	UPROPERTY( VisibleAnywhere, Category = "Gun Mesh" )
	USkeletalMeshComponent* SkeletalMesh;

	/** 총을 주웠는지 여부 체크 */
	bool IsPickedUp;

	/** 미사용 시 장착 소켓 위치 */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "EquipSocketName" )
	FName EquipSocketName;

	/* 총 아이콘 리턴 */
	UFUNCTION( BlueprintCallable )
	UTexture2D* GetGunIcon();

private:

	/* 총 Property */
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

	/* 총 이펙트 */
	UPROPERTY( EditAnywhere, Category = "Effects" )
	UParticleSystem* MuzzleEffect;

	UPROPERTY( EditAnywhere, Category = "Effects" )
	UParticleSystem* ImpactEffect;

	/* 총 사운드 */
	UPROPERTY( EditAnywhere, Category = "Sound" )
	USoundBase* OutOfAmmoSound;

	UPROPERTY( EditAnywhere, Category = "Sound" )
	USoundBase* FireSound;

	UPROPERTY( EditAnywhere, Category = "Sound" )
	USoundBase* ImpactSound;

	UPROPERTY( EditAnywhere, Category = "Sound" )
	USoundBase* ReloadingSound;
	
	/* 위젯에서 사용할 총 아이콘 */
	UPROPERTY( EditAnywhere, Category = "GunIcon" )
	UTexture2D* GunIcon;

public:
	/* 캐릭터 애니메이션 */
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

	/* 총을 소유한 폰과 해당 폰의 컨트롤러 */
	APawn* OwnerPawn;
	AController* OwnerController;
};
