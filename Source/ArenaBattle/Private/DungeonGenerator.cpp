// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonGenerator.h"
#include "SectionController.h"
#include "ABSection.h"
#include "ABGameMode.h"

// Sets default values
ADungeonGenerator::ADungeonGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//SectionController = CreateDefaultSubobject<USectionController>(TEXT("SectionController"));
	
	DirectionMovementMap.Add(0, FIntVector(1, 0, 0));
	DirectionMovementMap.Add(1, FIntVector(-1, 0, 0));
	DirectionMovementMap.Add(2, FIntVector(0, 1, 0));
	DirectionMovementMap.Add(3, FIntVector(0, -1, 0));
}

// Called when the game starts or when spawned
void ADungeonGenerator::BeginPlay()
{
	Super::BeginPlay();
	
	DungeonGenerationData.NumberOfCrawlers = NumberOfCrawlers;
	DungeonGenerationData.IterationMin = IterationMin;
	DungeonGenerationData.IterationMax = IterationMax;

	SectionController = GetWorld()->SpawnActor<ASectionController>(SectionControllerClass, FVector::ZeroVector, FRotator::ZeroRotator);
	ABCHECK(nullptr != SectionController);

	SpawnSectioin(DungeonSections);
}

// Called every frame
void ADungeonGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADungeonGenerator::InformOpenDoor(AABSection* OppositeSection)
{
	FIntVector Opposite = FIntVector(OppositeSection->GetRoomInfo()->X, OppositeSection->GetRoomInfo()->Y, 0);
	for (int i = 0; i < ABSections.Num(); i++)
	{
		FIntVector Section = FIntVector(ABSections[i]->GetRoomInfo()->X, ABSections[i]->GetRoomInfo()->Y, 0);

		for (int32 j = 0; j < DirectionMovementMap.Num(); j++)
		{
			if (Section == Opposite + DirectionMovementMap[j])
			{
				ABSections[i]->OpenGate(DirectionMovementMap[j] * -1, true);
			}
		}
	}
}

void ADungeonGenerator::InformCloseDoor(AABSection * Section)
{
	ABLOG(Warning, TEXT("Inform."));
	FIntVector OppositeVector = FIntVector(Section->GetRoomInfo()->X, Section->GetRoomInfo()->Y, 0);
	for (int i = 0; i < ABSections.Num(); i++)
	{
		FIntVector SectionVector = FIntVector(ABSections[i]->GetRoomInfo()->X, ABSections[i]->GetRoomInfo()->Y, 0);
		for (int32 j = 0; j < DirectionMovementMap.Num(); j++)
		{
			if (SectionVector == OppositeVector + DirectionMovementMap[j])
			{
				ABSections[i]->OpenGate(DirectionMovementMap[j] * -1, false);
				ABLOG(Warning, TEXT("CloseDoor."));
			}
		}
	}

}

FIntVector ADungeonGenerator::Move(AABSection* ABSection)
{
	int32 SelectIndex = FMath::RandRange(0, ABSection->GetConnectedDoors().Num() - 1);

	return DirectionMovementMap[ABSection->GetConnectedDoors()[SelectIndex]];
}

void ADungeonGenerator::SpawnSectioin(TArray<FIntVector> SectionsLocation)
{
	int32 SectionCount = FMath::RandRange(DungeonGenerationData.IterationMin, DungeonGenerationData.IterationMax);

	AABSection* StartSection = GetWorld()->SpawnActor<AABSection>(ABSectionClass, FVector::ZeroVector, FRotator::ZeroRotator);
	StartSection->SetDungeonGenerator(this);
	SectionController->LoadSection(StartSection, "0", 0, 0);
	ABSections.Add(0, StartSection);
	//¹®Á¦
	FIntVector Direction = Move(StartSection);
	StartSection->SetbConnectedDoors(Direction);
	int32 PositionX = 0;
	int32 PositionY = 0;

	int32 i = 1;
	for (int32 i = 1; i < 10; i++)
	{
		FString NewName = FString::FromInt(i);
		AABSection* NewSection = GetWorld()->SpawnActor<AABSection>(ABSectionClass, FVector::ZeroVector, FRotator::ZeroRotator);
		NewSection->SetDungeonGenerator(this);
		SectionController->LoadSection(NewSection, NewName, PositionX + Direction.X, PositionY + Direction.Y);
		ABSections.Add(i, NewSection);
		NewSection->SetbConnectedDoors(FIntVector(Direction.X * -1, Direction.Y * -1, 0));
		CheckAroundSection(NewSection);
		
		if (i < 9)
		{
			AABSection* SelectSection = GetRandomSection();
			Direction = Move(SelectSection);
			SelectSection->SetbConnectedDoors(Direction);
			PositionX = SelectSection->GetRoomInfo()->X;
			PositionY = SelectSection->GetRoomInfo()->Y;
		}	
	}

	UnConnectedDoors();
	auto* ABGameMode = Cast<AABGameMode>(GetWorld()->GetAuthGameMode());
	ABGameMode->SetDungeonGenerator(this);
}

void ADungeonGenerator::CheckAroundSection(AABSection* NextSection)
{
	FIntVector NextIntVector = FIntVector(NextSection->GetRoomInfo()->X, NextSection->GetRoomInfo()->Y, 0);

	for (int32 i = 0; i < ABSections.Num(); i++)
	{
		FIntVector PrevIntVector = FIntVector(ABSections[i]->GetRoomInfo()->X, ABSections[i]->GetRoomInfo()->Y, 0);

		for (int32 j = 0; j < DirectionMovementMap.Num(); j++)
		{
			if (NextIntVector + DirectionMovementMap[j] == PrevIntVector)
			{
				ABSections[i]->SetUnConnectedDoors(FIntVector(DirectionMovementMap[j].X * -1, DirectionMovementMap[j].Y * -1, 0));
				NextSection->SetUnConnectedDoors(DirectionMovementMap[j]);
			}
		}
	}
}

AABSection* ADungeonGenerator::GetRandomSection()
{
	while (true)
	{
		AABSection* SelectSection = ABSections[FMath::RandRange(0, ABSections.Num() - 1)];
		if (SelectSection->GetConnectedDoors().Num() != 0)
		{
			return SelectSection;
		}
	}

	return nullptr;
}

void ADungeonGenerator::UnConnectedDoors()
{
	for (int32 i = 0; i < ABSections.Num(); i++)
	{
		ABSections[i]->UnConnectedDoors();
	}
}

TMap<int, class AABSection*> ADungeonGenerator::GetABSections()
{
	return ABSections;
}


