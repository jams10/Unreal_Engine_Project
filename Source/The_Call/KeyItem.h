// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickupItem.h"
#include "KeyItem.generated.h"


UENUM( BlueprintType )
enum class EKeyType : uint8
{
	KT_NOTHING UMETA( DisplayName = "Nothing" ),
	KT_RED UMETA( DisplayName = "Red" ),
	KT_BLUE UMETA( DisplayName = "Blue" ),

	KT_MAX UMETA( DisplayName = "MAX" )
};

UCLASS()
class THE_CALL_API AKeyItem : public APickupItem
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AKeyItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick( float DeltaTime ) override;

public:

	void UseItem( class AShooterCharacter* PlayerCharacter ) override;

	UFUNCTION( BlueprintCallable )
	EKeyType GetKeyType() const;

	void OnOverlapBegin( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult ) override;

	void OnOverlapEnd( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex ) override;

public:

	UPROPERTY( VisibleAnywhere, BlueprintReadWrite, Category = "Key State" )
	bool bPlayerHasUsed;

private:

	UPROPERTY( EditAnywhere, Category = "KeyState" )
	EKeyType KeyType;

};
