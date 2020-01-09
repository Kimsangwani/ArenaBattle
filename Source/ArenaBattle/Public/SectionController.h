// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/Actor.h"
#include "SectionController.generated.h"

USTRUCT(BlueprintType)
struct FRoomInfo
{
	GENERATED_BODY()
public:

	FRoomInfo() : Name(" "), X(0), Y(0) {}

	FRoomInfo(const FString _Name, int32 _X, int32 _Y)
		: Name(_Name), X(_X), Y(_Y)
	{}

	UPROPERTY(EditAnywhere, Category = "SectionInfo")
		FString Name;

	UPROPERTY(EditAnywhere, Category = "SectionInfo")
		int32 X;

	UPROPERTY(EditAnywhere, Category = "SectionInfo")
		int32 Y;
};

UCLASS()
class ARENABATTLE_API ASectionController : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASectionController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	private:
	TQueue<struct FRoomInfo*> LoadSectionQueue;

	FRoomInfo* CurrentLoadSectionInfo = nullptr;

	bool bIsLoadingSection = false;

private:
	TArray<class AABSection*> LoadedSections;
	
private:
	void UpdateRoomQueue();

public:
	TArray<class AABSection*> GetLoadedSections() { return LoadedSections; }

	bool DoesSectionExist(int32 X, int32 Y);

	AABSection* FindSection(int32 X, int32 Y);

	void LoadSection(AABSection* NewSection, FString Name, int32 X, int32 Y);

	void RegisterSection(AABSection* ABSection);

	

};
