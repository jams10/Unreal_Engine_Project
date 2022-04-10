// Fill out your copyright notice in the Description page of Project Settings.


#include "HealingItem.h"
#include "ShooterCharacter.h"

#include "Components/SphereComponent.h"
#include "Components/SceneComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Kismet/GameplayStatics.h"

AHealingItem::AHealingItem()
{
	ItemType = EItemType::IT_HEALINGITEM;

	HealingAmount = 10.f;
}

void AHealingItem::BeginPlay()
{
	Super::BeginPlay();

	CollisionVolume->OnComponentBeginOverlap.AddDynamic( this, &AHealingItem::OnOverlapBegin );
	CollisionVolume->OnComponentEndOverlap.AddDynamic( this, &AHealingItem::OnOverlapEnd );
}

void AHealingItem::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

float AHealingItem::GetHealingAmount() const
{
	return HealingAmount;
}

void AHealingItem::UseItem( AShooterCharacter* PlayerCharacter )
{
	PlayerCharacter->IncreaseHealth( HealingAmount );
	UGameplayStatics::PlaySoundAtLocation( GetWorld(), UseSound, PlayerCharacter->GetActorLocation() );
	
}

void AHealingItem::OnOverlapBegin( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult )
{
	Super::OnOverlapBegin( OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult );

	UE_LOG( LogTemp, Warning, TEXT( "HealingItem OnOverlapBegin()" ) );
}

void AHealingItem::OnOverlapEnd( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex )
{
	Super::OnOverlapEnd( OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex );

	UE_LOG( LogTemp, Warning, TEXT( "HealingItem OnOverlapEnd()" ) );
}