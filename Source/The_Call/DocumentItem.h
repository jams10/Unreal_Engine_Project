// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickupItem.h"
#include "DocumentItem.generated.h"

/**
 * 
 */
UCLASS()
class THE_CALL_API ADocumentItem : public APickupItem
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ADocumentItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick( float DeltaTime ) override;

public:

	void UseItem( class AShooterCharacter* PlayerCharacter ) override;

	void OnOverlapBegin( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult ) override;

	void OnOverlapEnd( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex ) override;

public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Widgets" )
	TSubclassOf<UUserWidget> DocumentWidgetAsset;

	UPROPERTY( VisibleAnywhere, BlueprintReadWrite, Category = "Widget" )
	UUserWidget* DocumentWidget;
};
