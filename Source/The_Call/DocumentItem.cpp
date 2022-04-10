// Fill out your copyright notice in the Description page of Project Settings.


#include "DocumentItem.h"

#include "Blueprint/UserWidget.h"
#include "Components/SphereComponent.h"
#include "Components/SceneComponent.h"
#include "Components/PrimitiveComponent.h"

ADocumentItem::ADocumentItem()
{

}

void ADocumentItem::BeginPlay()
{
	Super::BeginPlay();
	
	CollisionVolume->OnComponentBeginOverlap.AddDynamic( this, &ADocumentItem::OnOverlapBegin );
	CollisionVolume->OnComponentEndOverlap.AddDynamic( this, &ADocumentItem::OnOverlapEnd );

	if( DocumentWidgetAsset )
	{
		DocumentWidget = CreateWidget( GetWorld()->GetGameInstance(), DocumentWidgetAsset );
	}
}

void ADocumentItem::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}


void ADocumentItem::UseItem( AShooterCharacter* PlayerCharacter )
{
	
}

void ADocumentItem::OnOverlapBegin( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult )
{
	Super::OnOverlapBegin( OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult );

	UE_LOG( LogTemp, Warning, TEXT( "DocumentItem OnOverlapBegin()" ) );
}

void ADocumentItem::OnOverlapEnd( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex )
{
	Super::OnOverlapEnd( OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex );

	UE_LOG( LogTemp, Warning, TEXT( "DocumentItem OnOverlapEnd()" ) );
}