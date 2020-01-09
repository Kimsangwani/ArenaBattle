// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/PlayerController.h"
#include "ABPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AABPlayerController();

	virtual void PostInitializeComponents() override;
	virtual void OnPossess(APawn* aPawn) override;

	class UABHUDWidget* GetHUDWidget() const;
	void NPCKill(class AABCharacter* KilledNPC) const;
	void AddGameScore() const;

	void ChangeInputMode(bool bGameMode = true);
	void ShowResultUI();
	
	class UGamePlayMiniMap* GetGamePlayMiniMap() const;

	UPROPERTY()
	int32 index = 0;
protected:
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
		TSubclassOf<class UABHUDWidget> HUDWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
		TSubclassOf<class UABGameplayWidget> MenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
		TSubclassOf<class UABGameplayWidget> ResultWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
		TSubclassOf<class UGamePlayMiniMap> MiniMapWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
		TSubclassOf<class UUserWidget> SectionWidgetClass;

private:
	UPROPERTY()
		class UABHUDWidget* HUDWidget;

	UPROPERTY()
		class AABPlayerState* ABPlayerState;

	UPROPERTY()
		class UABGameplayWidget* MenuWidget;

	UPROPERTY()
		class UABGameplayResultWidget* ResultWidget;

	UPROPERTY()
		class UGamePlayMiniMap* MiniMapWidget;

	UPROPERTY()
		class UUserWidget* SectionWidget;

	FInputModeGameOnly GameInputMode;
	FInputModeUIOnly UIInputMode;



private:
	void OnGamePause();
	void OnMiniMap();
	void CreateSectionImage();
	
};
