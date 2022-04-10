// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoItem.h"

#include "Components/SphereComponent.h"
#include "Components/SceneComponent.h"
#include "Components/PrimitiveComponent.h"

AAmmoItem::AAmmoItem()
{
	ItemType = EItemType::IT_AMMO;

	StackAmount = 0;
	AmmoType = EAmmoType::AT_NOTHING;
}

void AAmmoItem::BeginPlay()
{
	Super::BeginPlay();

	CollisionVolume->OnComponentBeginOverlap.AddDynamic( this, &AAmmoItem::OnOverlapBegin );
	CollisionVolume->OnComponentEndOverlap.AddDynamic( this, &AAmmoItem::OnOverlapEnd );
}

void AAmmoItem::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

void AAmmoItem::UseItem( AShooterCharacter* PlayerCharacter )
{

}
	
void AAmmoItem::OnOverlapBegin( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult )
{
	Super::OnOverlapBegin( OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult );

	UE_LOG( LogTemp, Warning, TEXT( "AmmoItem OnOverlapBegin()" ) );
}

void AAmmoItem::OnOverlapEnd( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex )
{
	Super::OnOverlapEnd( OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex );

	UE_LOG( LogTemp, Warning, TEXT( "AmmoItem OnOverlapEnd()" ) );
}