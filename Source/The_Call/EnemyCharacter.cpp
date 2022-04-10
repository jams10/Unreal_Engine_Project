// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "ShooterCharacter.h"
#include "AIController.h"

#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "Engine/EngineTypes.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Sound/SoundBase.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;

	AttackDamage = 20.0f;

	bIsAttacking = false;
	bPlayerIsInAttackRange = false;

	AttackRangeCollision = CreateDefaultSubobject<USphereComponent>( TEXT( "AttackRangeCollision" ) );
	AttackRangeCollision->SetupAttachment( GetRootComponent() );
	AttackRangeCollision->InitSphereRadius( 90.f );

	AttackCollision = CreateDefaultSubobject<UBoxComponent>( TEXT( "AttackCollision" ) );
	AttackCollision->SetupAttachment( GetMesh(), FName( "EnemyAttackSocket" ) );

	AttackCollision->SetCollisionEnabled( ECollisionEnabled::NoCollision );
	AttackCollision->SetCollisionObjectType( ECollisionChannel::ECC_WorldDynamic );

	// 모든 채널에 대해 Ignore
	AttackCollision->SetCollisionResponseToAllChannels( ECollisionResponse::ECR_Ignore );
	// Pawn 채널에 대해서만 Overlap 대응하도록 함.
	AttackCollision->SetCollisionResponseToChannel( ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap );

	IsGettingHit = false;
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;

	AIController = Cast<AAIController>( GetController() );
	
	AttackRangeCollision->OnComponentBeginOverlap.AddDynamic( this, &AEnemyCharacter::AttackRangeOnOverlapBegin );
	AttackRangeCollision->OnComponentEndOverlap.AddDynamic( this, &AEnemyCharacter::AttackRangeOnOverlapEnd );

	AttackCollision->OnComponentBeginOverlap.AddDynamic( this, &AEnemyCharacter::AttackOnOverlapBegin );
	AttackCollision->OnComponentEndOverlap.AddDynamic( this, &AEnemyCharacter::AttackOnOverlapEnd );

}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if( IsDead() && DeathMontage )
	{
		AttackRangeCollision->SetCollisionEnabled( ECollisionEnabled::NoCollision );
		AttackCollision->SetCollisionEnabled( ECollisionEnabled::NoCollision );
		GetMesh()->SetCollisionEnabled( ECollisionEnabled::NoCollision );
		GetCapsuleComponent()->SetCollisionEnabled( ECollisionEnabled::NoCollision );
	}
}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AEnemyCharacter::TakeDamage( float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser )
{
	float DamageToApply = Super::TakeDamage( DamageAmount, DamageEvent, EventInstigator, DamageCauser );
	
	DamageToApply = FMath::Min( CurrentHealth, DamageToApply );

	CurrentHealth -= DamageToApply;

	UGameplayStatics::PlaySoundAtLocation( GetWorld(), GettingHitVoiceSound, GetActorLocation() );
	UGameplayStatics::PlaySoundAtLocation( GetWorld(), GettingHitBodySound, GetActorLocation() );

	UE_LOG( LogTemp, Warning, TEXT( "NAME : %s / HEALTH : %f" ), *GetName(), CurrentHealth );

	return DamageToApply;
}

bool AEnemyCharacter::IsDead() const
{
	return CurrentHealth <= 0.f;
}

void AEnemyCharacter::AttackPlayerStart()
{
	if( !bIsAttacking )
	{
		bIsAttacking = true;
		UE_LOG( LogTemp, Warning, TEXT( "attack!" ) );
		
		if( AttackMontage )
		{
			PlayAnimMontage( AttackMontage, 1.2f );
		}
	}
}

void AEnemyCharacter::AttackPlayerEnd()
{
	bIsAttacking = false;
}

void AEnemyCharacter::TurnOnAttackCollision()
{
	AttackCollision->SetCollisionEnabled( ECollisionEnabled::QueryOnly );
}


void AEnemyCharacter::TurnOffAttackCollision()
{
	AttackCollision->SetCollisionEnabled( ECollisionEnabled::NoCollision );
}

bool AEnemyCharacter::IsInNormalState()
{
	return !IsGettingHit && !bIsAttacking;
}

void AEnemyCharacter::AttackRangeOnOverlapBegin( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult )
{
	if( OtherActor )
	{
		AShooterCharacter* PlayerCharacter = Cast<AShooterCharacter>( OtherActor );
		if( PlayerCharacter )
		{
			UE_LOG( LogTemp, Warning, TEXT( "INRANGE" ) );
			bPlayerIsInAttackRange = true;
		}
	}
}

void AEnemyCharacter::AttackRangeOnOverlapEnd( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex )
{
	bPlayerIsInAttackRange = false;
}

void AEnemyCharacter::AttackOnOverlapBegin( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult )
{
	UE_LOG( LogTemp, Warning, TEXT( "AttackOverlap1" ) );
	if( OtherActor )
	{
		AShooterCharacter* PlayerCharacter = Cast<AShooterCharacter>( OtherActor );
		if( PlayerCharacter )
		{
			UE_LOG( LogTemp, Warning, TEXT( "AttackOverlap2" ) );
			
			if( DamageTypeClass )
			{
				UGameplayStatics::ApplyDamage( PlayerCharacter, AttackDamage, AIController, this, DamageTypeClass );
			}
		}
	}
}

void AEnemyCharacter::AttackOnOverlapEnd( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex )
{

}

void AEnemyCharacter::ResetDeathTimer()
{
	UE_LOG( LogTemp, Warning, TEXT( "Destroy!" ) );
	Destroy();
}

