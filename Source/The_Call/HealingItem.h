// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickupItem.h"
#include "HealingItem.generated.h"

/**
 * 
 */

UCLASS()
class THE_CALL_API AHealingItem : public APickupItem
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AHealingItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick( float DeltaTime ) override;

public:

	float GetHealingAmount() const;

	void UseItem( class AShooterCharacter* PlayerCharacter ) override;

	void OnOverlapBegin( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult ) override;

	void OnOverlapEnd( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex ) override;

private:

	UPROPERTY( EditAnywhere, Category = "HealingProperty" )
	float HealingAmount;
};