// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickupItem.h"
#include "AmmoItem.generated.h"

/**
 * 
 */

UENUM( BlueprintType )
enum class EAmmoType : uint8
{
	AT_NOTHING UMETA( DisplayName = "Nothing" ),
	AT_PISTOL UMETA( DisplayName = "Pistol" ),
	AT_RIFLE UMETA( DisplayName = "Rifle" ),

	AT_MAX UMETA( DisplayName = "MAX" )
};

UCLASS()
class THE_CALL_API AAmmoItem : public APickupItem
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAmmoItem();

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

	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite, Category = "AmmoProperty" )
	int32 StackAmount;

	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite, Category = "AmmoProperty" )
	EAmmoType AmmoType;
};
