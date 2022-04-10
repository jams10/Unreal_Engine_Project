// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterPlayerController.h"

#include "PickupItem.h"
#include "Blueprint/UserWidget.h"

void AShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	/** 에셋이 할당 되었는지 체크 */
	if( MainHUDAsset )
	{
		MainHUD = CreateWidget<UUserWidget>( this, MainHUDAsset );
	}
	MainHUD->AddToViewport();
	MainHUD->SetVisibility( ESlateVisibility::Visible );
}