// Fill out your copyright notice in the Description page of Project Settings.

#include "ABPlayerController.h"
#include "ABHUDWidget.h"
#include "ABPlayerState.h"
#include "ABCharacter.h"
#include "ABGameplayWidget.h"
#include "ABGameplayResultWidget.h"
#include "ABGameState.h"
#include "GamePlayMiniMap.h"
#include "Components/WidgetComponent.h"
#include "WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"

AABPlayerController::AABPlayerController()
{
	static ConstructorHelpers::FClassFinder<UABHUDWidget> UI_HUD_C(TEXT("/Game/Book/UI/UI_HUD.UI_HUD_C"));
	if (UI_HUD_C.Succeeded())
	{
		HUDWidgetClass = UI_HUD_C.Class;
	}

	static ConstructorHelpers::FClassFinder<UABGameplayWidget> UI_MENU_C(TEXT("/Game/Book/UI/UI_Menu.UI_Menu_C"));
	if (UI_MENU_C.Succeeded())
	{
		MenuWidgetClass = UI_MENU_C.Class;
	}

	static ConstructorHelpers::FClassFinder<UABGameplayResultWidget> UI_RESULT_C(TEXT("/Game/Book/UI/UI_Result.UI_Result_C"));
	if (UI_RESULT_C.Succeeded())
	{
		ResultWidgetClass = UI_RESULT_C.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> UI_SECTION_C(TEXT("/Game/Book/UI/UI_Section.UI_Section_C"));
	if (UI_SECTION_C.Succeeded())
	{
		SectionWidgetClass = UI_SECTION_C.Class;
	}

	static ConstructorHelpers::FClassFinder<UGamePlayMiniMap> UI_MINIMAP_C(TEXT("/Game/Book/UI/UI_MiniMap.UI_MiniMap_C"));
	if (UI_MINIMAP_C.Succeeded())
	{
		MiniMapWidgetClass = UI_MINIMAP_C.Class;
	}

	index = 0;
}

void AABPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ABLOG_S(Warning);
}

void AABPlayerController::OnPossess(APawn* aPawn)
{
	ABLOG_S(Warning);
	Super::OnPossess(aPawn);
}

void AABPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction(TEXT("GamePause"), EInputEvent::IE_Pressed, this, &AABPlayerController::OnGamePause);
	InputComponent->BindAction(TEXT("MiniMap"), EInputEvent::IE_Pressed, this, &AABPlayerController::OnMiniMap);
}

void AABPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ChangeInputMode(true);

	HUDWidget = CreateWidget<UABHUDWidget>(this, HUDWidgetClass);
	ABCHECK(nullptr != HUDWidget);
	HUDWidget->AddToViewport(1);

	ResultWidget = CreateWidget<UABGameplayResultWidget>(this, ResultWidgetClass);
	ABCHECK(nullptr != ResultWidgetClass);

	ABPlayerState = Cast<AABPlayerState>(PlayerState);
	ABCHECK(nullptr != ABPlayerState);
	HUDWidget->BindPlayerState(ABPlayerState);
	ABPlayerState->OnPlayerStateChanged.Broadcast();

	if (nullptr == MiniMapWidget)
	{
		MiniMapWidget = CreateWidget<UGamePlayMiniMap>(this, MiniMapWidgetClass);
		ABCHECK(nullptr != MiniMapWidget);

		MiniMapWidget->AddToViewport(3);
		MiniMapWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AABPlayerController::OnGamePause()
{
	MenuWidget = CreateWidget<UABGameplayWidget>(this, MenuWidgetClass);
	ABCHECK(nullptr != MenuWidget);
	MenuWidget->AddToViewport(4);

	SetPause(true);
	ChangeInputMode(false);
}

void AABPlayerController::OnMiniMap()
{ 
	//AABCharacter* Player = Cast<AABCharacter>(this->GetCharacter());
	//ABLOG(Warning, TEXT("%d %d"), Player->GetActorLocation().X, Player->GetActorLocation().Y);

	if (MiniMapWidget->IsVisible())
	{
		MiniMapWidget->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		MiniMapWidget->SetVisibility(ESlateVisibility::Visible);
	}
	//ChangeInputMode(false);
}

void AABPlayerController::CreateSectionImage()
{

}

UABHUDWidget* AABPlayerController::GetHUDWidget() const
{
	return HUDWidget;
}

void AABPlayerController::NPCKill(AABCharacter * KilledNPC) const
{
	ABPlayerState->AddExp(KilledNPC->GetExp());
}

void AABPlayerController::AddGameScore() const
{
	ABPlayerState->AddGameScore();
}

void AABPlayerController::ChangeInputMode(bool bGameMode)
{
	if (bGameMode)
	{
		SetInputMode(GameInputMode);
		bShowMouseCursor = false;
	}
	else
	{
		SetInputMode(UIInputMode);
		bShowMouseCursor = true;
	}
}

void AABPlayerController::ShowResultUI()
{
	MiniMapWidget->RemoveFromParent();
	auto ABGameState = Cast<AABGameState>(UGameplayStatics::GetGameState(this));
	ABCHECK(nullptr != ABGameState);
	ResultWidget->BindGameState(ABGameState);

	ResultWidget->AddToViewport();
	ChangeInputMode(false);
}

UGamePlayMiniMap* AABPlayerController::GetGamePlayMiniMap() const
{
	return MiniMapWidget;
}


