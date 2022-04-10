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

	/** 오버랩 이벤트를 위한 구체 콜리젼 */
	UPROPERTY( VisibleAnywhere, BlueprintReadWrite, Category = "Collision" )
	class USphereComponent* CollisionVolume;

	/** 아이템 그림 */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "ItemInfo" )
	class UTexture2D* ItemImage;

	/** 아이템 이름 */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "ItemInfo" )
	FText ItemName;

	/** 아이템 액션 */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "ItemInfo" )
	FText ActionText;

	/** 아이템 설명 */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "ItemInfo" )
	FText ItemDescription;

	/** 아이템 타입 */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "ItemInfo" )
	EItemType ItemType;

	/** 줍기 표시 텍스트 위젯 */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Widgets" )
	TSubclassOf<UUserWidget> PickupTextWidgetAsset;
	UPROPERTY( VisibleAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* PickupTextWidget;

	/** 아이템 습득 사운드 */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Sound" )
	USoundBase* PickupSound;

	/** 아이템 사용 사운드 */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Sound" )
	USoundBase* UseSound;

	/** 줍기 표시 텍스트 렌더 컴포넌트 */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Display" )
	class UTextRenderComponent* TextRenderComponent;

	/** 캐릭터가 범위 안에 있는지 여부를 나타내는 변수 */
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "OverlapEvent" )
	bool bIsInRange;

	UFUNCTION( BlueprintCallable )
	virtual void UseItem( class AShooterCharacter* PlayerCharacter );

	UFUNCTION()
	virtual void OnOverlapBegin( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult );
	UFUNCTION()
	virtual void OnOverlapEnd( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex );

};
