// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupItem.h"
#include "ShooterCharacter.h"

#include "Blueprint/UserWidget.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/Texture.h"
#include "Engine/Texture2D.h"
#include "Internationalization/Text.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

// Sets default values
APickupItem::APickupItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Root Component 생성
	Root = CreateDefaultSubobject<USceneComponent>( TEXT( "Root" ) );
	SetRootComponent( Root );

	// StaticMesh Component 생성
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "StaticMesh" ) );
	StaticMesh->SetupAttachment( Root );

	// 콜리젼 생성
	CollisionVolume = CreateDefaultSubobject<USphereComponent>( TEXT( "OverlapCollision" ) );
	CollisionVolume->SetupAttachment( Root );

	ItemType = EItemType::IT_NOTHING;

	bIsInRange = false;
}

// Called when the game starts or when spawned
void APickupItem::BeginPlay()
{
	Super::BeginPlay();
	
	if( PickupTextWidgetAsset )
	{
		PickupTextWidget = CreateWidget( GetWorld()->GetGameInstance(), PickupTextWidgetAsset );
		PickupTextWidget->AddToViewport();
		PickupTextWidget->SetVisibility( ESlateVisibility::Hidden );
	}
}

// Called every frame
void APickupItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/** 아이템 픽업 텍스트 위젯의 텍스트가 플레이어를 바라보게 설정함 */
	FVector ItemLocation = GetActorLocation();
	ItemLocation.Z += 50.f;

	FVector2D ScreenLocation;
	if( PickupTextWidget && UGameplayStatics::ProjectWorldToScreen( UGameplayStatics::GetPlayerController( GetWorld(), 0 ), ItemLocation, ScreenLocation ) )
	{
		PickupTextWidget->SetPositionInViewport( ScreenLocation );
	}
}

void APickupItem::UseItem( class AShooterCharacter* PlayerCharacter )
{
	UE_LOG( LogTemp, Warning, TEXT( "USE ITEM(CALL FROM PICKUPTIEM)" ) );
}

void APickupItem::OnOverlapBegin( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult )
{
	if( OtherActor )
	{
		UE_LOG( LogTemp, Warning, TEXT( "PickupItem OnOverlapBegin()" ) );
		AShooterCharacter* ShooterCharater = Cast<AShooterCharacter>( OtherActor );
		
		if( ShooterCharater && PickupTextWidget )
		{
			PickupTextWidget->SetVisibility( ESlateVisibility::Visible );
			bIsInRange = true;
		}
	}
}

void APickupItem::OnOverlapEnd( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex )
{
	if( OtherActor )
	{
		UE_LOG( LogTemp, Warning, TEXT( "PickupItem OnOverlapEnd()" ) );
		AShooterCharacter* ShooterCharater = Cast<AShooterCharacter>( OtherActor );

		if( ShooterCharater && PickupTextWidget )
		{
			PickupTextWidget->SetVisibility( ESlateVisibility::Hidden );
			bIsInRange = false;
		}
	}
}

