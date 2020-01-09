// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Blueprint/UserWidget.h"
#include "GamePlayMiniMap.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UGamePlayMiniMap : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick();
public:
	void CreateSection();
	void CurrentImageIndex(int32 Index);
private:
	UPROPERTY()
		class UCanvasPanel* RootPanel;
	UPROPERTY()
		TArray<class UImage*> SectionImages;

	bool bFlashSection = false;
	FTimerHandle FlashTimerHandle = { };

	int32 ImgIndex;
	float time;

private:

};
