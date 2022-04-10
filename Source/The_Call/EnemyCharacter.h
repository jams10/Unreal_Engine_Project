// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class THE_CALL_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage( float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser ) override;

	UFUNCTION( BlueprintCallable )
	bool IsDead() const;

	// 블루프린트에서 구현부분을 작성하기 위해 BlueprintImplementableEvent 파라미터를 줌.
	UFUNCTION( BlueprintImplementableEvent, BlueprintCallable, Category = "Hit" )
	void ReactionByHit( FName BoneName, FVector Location );

	UFUNCTION( BlueprintCallable )
	void AttackPlayerStart();

	UFUNCTION( BlueprintCallable )
	void AttackPlayerEnd();

	UFUNCTION( BlueprintCallable )
	void TurnOnAttackCollision();

	UFUNCTION( BlueprintCallable )
	void TurnOffAttackCollision();

	UFUNCTION( BlueprintCallable )
	bool IsInNormalState();

public:

	/** 공격 범위를 위한 콜리젼 */
	UPROPERTY( VisibleAnywhere, BlueprintReadWrite, Category = "Attack" )
	class USphereComponent* AttackRangeCollision;

	/** 공격을 위한 콜리젼 */
	UPROPERTY( VisibleAnywhere, BlueprintReadWrite, Category = "Attack" )
	class UBoxComponent* AttackCollision;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Attack" )
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "AI" )
	class AAIController* AIController;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "EnemyState" )
	bool bIsAttacking;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "EnemyState" )
	bool bPlayerIsInAttackRange;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Particle" )
	class UParticleSystem* HitParticle;

public:
	/** 콜리젼 오버랩 이벤트 함수들 */
	UFUNCTION()
	void AttackRangeOnOverlapBegin( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult );

	UFUNCTION()
	void AttackRangeOnOverlapEnd( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex );

	UFUNCTION()
	void AttackOnOverlapBegin( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult );

	UFUNCTION()
	void AttackOnOverlapEnd( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex );

private:

	/* 적 캐릭터 체력 */
	UPROPERTY( EditDefaultsOnly, Category = "EnemyProperty" )
	float MaxHealth;
	
	float CurrentHealth;

	/* 적 캐릭터 데미지 */
	UPROPERTY( EditDefaultsOnly, Category = "EnemyProperty" )
	float AttackDamage;

	/** 애니메이션 몽타주 */
	UPROPERTY( EditAnywhere, Category = "Montage" )
	class UAnimMontage* AttackMontage;

	UPROPERTY( EditAnywhere, Category = "Montage" )
	class UAnimMontage* DeathMontage;

	class AShooterCharacter* AttackTarget;

	FTimerHandle DeathDelayTimerHandle;

	/** 사운드 */
	UPROPERTY( EditAnywhere, Category = "Sound" )
	USoundBase* GettingHitVoiceSound;

	UPROPERTY( EditAnywhere, Category = "Sound" )
	USoundBase* GettingHitBodySound;

	UPROPERTY( EditAnywhere, Category = "Sound" )
	USoundBase* AttackSound;

	UPROPERTY( EditAnywhere, Category = "Sound" )
	USoundBase* VoiceSound;

	bool IsGettingHit;

private:

	void ResetDeathTimer();
};
