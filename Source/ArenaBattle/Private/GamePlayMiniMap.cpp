// Fill out your copyright notice in the Description page of Project Settings.


#include "GamePlayMiniMap.h"
#include "WidgetTree.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "CanvasPanelSlot.h"
#include "ABGameState.h"
#include "ABGameMode.h"
#include "Slate/WidgetTransform.h"
#include "ABSection.h"
#include "DungeonGenerator.h"
#include "SectionController.h"
#include "ABPlayerController.h"

void UGamePlayMiniMap::NativeConstruct()
{
	Super::NativeConstruct();

	RootPanel = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("canvasRootPanel")));
	ABCHECK(nullptr != RootPanel);

	
	AABGameMode* GameMode = Cast<AABGameMode>(GetWorld()->GetAuthGameMode());
	ImgIndex = GameMode->GetScoreToClear() - 1;
	if (nullptr != GameMode)
	{
		for (int i = 0; i < 10; i++)
		{
			FString TheString = "Image" + FString::FromInt(i);
			UImage* ImgSection = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), *TheString);
			ImgSection->SetOpacity(0.5f);
			RootPanel->AddChild(ImgSection);

			UPanelSlot* Slot = RootPanel->AddChild(ImgSection);
			UCanvasPanelSlot* canvasSlot = Cast<UCanvasPanelSlot>(Slot);
			canvasSlot->SetAnchors(FAnchors(0.5f, 0.5f));
			canvasSlot->SetSize(FVector2D(100.0f, 50.0f));
			canvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));

			ImgSection->SetRenderTranslation(FVector2D(GameMode->GetDungeonGenerator()->GetABSections()[i]->GetRoomInfo()->Y  * 110.0f, GameMode->GetDungeonGenerator()->GetABSections()[i]->GetRoomInfo()->X * -60.0f));
			SectionImages.Add(ImgSection);
		}
	}

	SectionImages[0]->SetOpacity(0.5f);
}

void UGamePlayMiniMap::NativeTick()
{

}

void UGamePlayMiniMap::CreateSection()
{

}

void UGamePlayMiniMap::CurrentImageIndex(int32 Index)
{
	SectionImages[ImgIndex]->SetOpacity(0.5f);
	SectionImages[Index]->SetOpacity(1.0f);
	ImgIndex = Index;
}


