// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterPlayerController.h"

#include "PickupItem.h"
#include "Blueprint/UserWidget.h"

void AShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	/** ������ �Ҵ� �Ǿ����� üũ */
	if( MainHUDAsset )
	{
		MainHUD = CreateWidget<UUserWidget>( this, MainHUDAsset );
	}
	MainHUD->AddToViewport();
	MainHUD->SetVisibility( ESlateVisibility::Visible );
}