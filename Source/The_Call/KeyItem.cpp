// Fill out your copyright notice in the Description page of Project Settings.


#include "KeyItem.h"

#include "Components/SphereComponent.h"
#include "Components/SceneComponent.h"
#include "Components/PrimitiveComponent.h"

AKeyItem::AKeyItem()
{
	bPlayerHasUsed = false;
	KeyType = EKeyType::KT_NOTHING;
}

void AKeyItem::BeginPlay()
{
	Super::BeginPlay();

	CollisionVolume->OnComponentBeginOverlap.AddDynamic( this, &AKeyItem::OnOverlapBegin );
	CollisionVolume->OnComponentEndOverlap.AddDynamic( this, &AKeyItem::OnOverlapEnd );
}

void AKeyItem::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}


void AKeyItem::UseItem( AShooterCharacter* PlayerCharacter )
{
	bPlayerHasUsed = true;
}

EKeyType AKeyItem::GetKeyType() const
{
	return KeyType;
}

void AKeyItem::OnOverlapBegin( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult )
{
	Super::OnOverlapBegin( OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult );

	UE_LOG( LogTemp, Warning, TEXT( "KeyItem OnOverlapBegin()" ) );
}

void AKeyItem::OnOverlapEnd( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex )
{
	Super::OnOverlapEnd( OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex );

	UE_LOG( LogTemp, Warning, TEXT( "KeyItem OnOverlapEnd()" ) );
}