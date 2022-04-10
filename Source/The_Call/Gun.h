// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gun.generated.h"

UENUM( BlueprintType )
enum class EWeaponType : uint8
{
	WT_NOTHING UMETA(DisplayName = "Nothing"),
	WT_PISTOL UMETA(DisplayName = "Pistol"),
	WT_RIFLE UMETA(DisplayName = "Rifle"),
	
	WT_MAX UMETA(DisplayName = "MAX")
};

UCLASS()
class THE_CALL_API AGun : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGun();

	void PullTrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetWeaponVisibility( const bool& State );

	float GetShotDelayTime() const;

	float GetReloadDelayTime() const;

	EWeaponType GetWeaponType() const;

	int32 GetLoadedBulletNumber() const;

	void Reload( int32& Ammo );

	UFUNCTION()
	virtual void OnOverlapBegin( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult );
	UFUNCTION()
	virtual void OnOverlapEnd( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex );

public:

	/** 오버랩 이벤트를 위한 구체 콜리젼 */
	UPROPERTY( VisibleAnywhere, BlueprintReadWrite, Category = "Collision" )
	class USphereComponent* CollisionVolume;

	/* 루트 및 메쉬 컴포넌트 인스턴스 */
	UPROPERTY( VisibleAnywhere )
	USceneComponent* Root;

	UPROPERTY( VisibleAnywhere )
	USkeletalMeshComponent* SkeletalMesh;

	bool IsPickedUp;

	FTimerHandle PhysicsTimer;

	void ResetPhysicsTimer();

	class AEnemyCharacter* EnemyCharacter;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "EquipSocketName" )
	FName EquipSocketName;

private:
	 
	/* 총 Property */
	UPROPERTY( EditAnywhere, Category = "Property" )
	EWeaponType WeaponType;

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

	UPROPERTY( EditAnywhere, Category = "Effects" )
	USoundBase* OutOfAmmoSound;

	UPROPERTY( EditAnywhere, Category = "Effects" )
	USoundBase* FireSound;

	UPROPERTY( EditAnywhere, Category = "Effects" )
	USoundBase* ImpactSound;

public:
	/* 캐릭터 애니메이션 */
	UPROPERTY( EditAnywhere, Category = "UpperBodyMontage" )
	class UAnimMontage* FireAnimation;

	UPROPERTY( EditAnywhere, Category = "UpperBodyMontage" )
	class UAnimMontage* ReloadAnimation;

	UPROPERTY( EditAnywhere, Category = "UpperBodyMontage" )
	class UAnimMontage* SwapAnimation;

private:

	/* 총을 소유한 폰과 해당 폰의 컨트롤러 */
	APawn* OwnerPawn;
	AController* OwnerController;

};
