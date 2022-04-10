// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShooterPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class THE_CALL_API AShooterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	/** 에디터 상의 UMG 에셋에 대한 레퍼런스 */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Widgets" )
	TSubclassOf<class UUserWidget> MainHUDAsset;

	/** UMG 에셋을 통한 위젯 생성 후 해당 위젯을 저장해둘 변수 */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Widgets" )
	UUserWidget* MainHUD;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Inventory" )
	TArray<class APickupItem*> Inventory;

public:

	UFUNCTION( BlueprintImplementableEvent, BlueprintCallable, Category = "Inventory" )
	void RefreshInventory();

protected:
	virtual void BeginPlay() override;
};
