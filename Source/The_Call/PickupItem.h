// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupItem.generated.h"

UENUM( BlueprintType )
enum class EItemType : uint8
{
	IT_NOTHING UMETA( DisplayName = "Nothing" ),
	IT_AMMO UMETA( DisplayName = "Ammo" ),
	IT_WEAPON UMETA( DisplayName = "Weapon" ),
	IT_HEALINGITEM UMETA( DisplayName = "HealingItem" ),
	IT_KEY UMETA( DisplayName = "Key" ),

	IT_MAX UMETA( DisplayName = "MAX" )
};

UCLASS()
class THE_CALL_API APickupItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	
	UPROPERTY( VisibleAnywhere, BlueprintReadWrite )
	class USceneComponent* Root;
	
	UPROPERTY( VisibleAnywhere, BlueprintReadWrite, Category = "ItemInfo" )
	class UStaticMeshComponent* StaticMesh;

	/** ������ �̺�Ʈ�� ���� ��ü �ݸ��� */
	UPROPERTY( VisibleAnywhere, BlueprintReadWrite, Category = "Collision" )
	class USphereComponent* CollisionVolume;

	/** ������ �׸� */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "ItemInfo" )
	class UTexture2D* ItemImage;

	/** ������ �̸� */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "ItemInfo" )
	FText ItemName;

	/** ������ �׼� */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "ItemInfo" )
	FText ActionText;

	/** ������ ���� */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "ItemInfo" )
	FText ItemDescription;

	/** ������ Ÿ�� */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "ItemInfo" )
	EItemType ItemType;

	/** �ݱ� ǥ�� �ؽ�Ʈ ���� */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Widgets" )
	TSubclassOf<UUserWidget> PickupTextWidgetAsset;
	UPROPERTY( VisibleAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* PickupTextWidget;

	/** ������ ���� ���� */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Sound" )
	USoundBase* PickupSound;

	/** ������ ��� ���� */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Sound" )
	USoundBase* UseSound;

	/** �ݱ� ǥ�� �ؽ�Ʈ ���� ������Ʈ */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Display" )
	class UTextRenderComponent* TextRenderComponent;

	/** ĳ���Ͱ� ���� �ȿ� �ִ��� ���θ� ��Ÿ���� ���� */
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "OverlapEvent" )
	bool bIsInRange;

	UFUNCTION( BlueprintCallable )
	virtual void UseItem( class AShooterCharacter* PlayerCharacter );

	UFUNCTION()
	virtual void OnOverlapBegin( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult );
	UFUNCTION()
	virtual void OnOverlapEnd( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex );

};
