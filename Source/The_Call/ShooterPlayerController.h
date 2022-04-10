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
	/** ������ ���� UMG ���¿� ���� ���۷��� */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Widgets" )
	TSubclassOf<class UUserWidget> MainHUDAsset;

	/** UMG ������ ���� ���� ���� �� �ش� ������ �����ص� ���� */
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
