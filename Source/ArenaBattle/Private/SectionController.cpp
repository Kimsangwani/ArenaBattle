// Fill out your copyright notice in the Description page of Project Settings.


#include "SectionController.h"
#include "ABSection.h"

// Sets default values
ASectionController::ASectionController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASectionController::BeginPlay()
{
	Super::BeginPlay();
	

}

// Called every frame
void ASectionController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//UpdateRoomQueue();
}

void ASectionController::UpdateRoomQueue()
{
	if (bIsLoadingSection)
	{
		return;
	}

	if (LoadSectionQueue.IsEmpty())
	{
		return;
	}

	FRoomInfo* GetSectionInfo;
	LoadSectionQueue.Dequeue(GetSectionInfo);
	bIsLoadingSection = true;
}

bool ASectionController::DoesSectionExist(int32 X, int32 Y)
{
	int32 LoadedSectionX = 0;
	int32 LoadedSectionY = 0;
	for (auto LoadedSection : LoadedSections)
	{
		LoadedSectionX = LoadedSection->GetRoomInfo()->X;
		LoadedSectionY = LoadedSection->GetRoomInfo()->Y;
		if (LoadedSectionX == X && LoadedSectionY == Y)
		{
			return true;
		}
	}

	return false;
}

AABSection* ASectionController::FindSection(int32 X, int32 Y)
{
	int32 LoadedSectionX = 0;
	int32 LoadedSectionY = 0;
	for (auto LoadedSection : LoadedSections)
	{
		LoadedSectionX = LoadedSection->GetRoomInfo()->X;
		LoadedSectionY = LoadedSection->GetRoomInfo()->Y;
		if (LoadedSectionX == X && LoadedSectionY == Y)
		{
			return LoadedSection;
		}
	}

	return nullptr;
}

void ASectionController::LoadSection(AABSection* NewSection, FString Name, int32 X, int32 Y)
{
	FRoomInfo* NewRoomInfo = new FRoomInfo(Name, X, Y);
	 
	NewSection->SetRoomInfo(NewRoomInfo);
	RegisterSection(NewSection);
}

void ASectionController::RegisterSection(AABSection* ABSection)
{
	if (nullptr == ABSection)
		return;
	ABSection->SetActorLocation(FVector(ABSection->GetRoomInfo()->X * ABSection->GetRoomSize().X, ABSection->GetRoomInfo()->Y * ABSection->GetRoomSize().Y, 0.0f));

	LoadedSections.Add(ABSection);
}



