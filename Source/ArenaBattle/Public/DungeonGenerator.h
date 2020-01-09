// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DungeonGenerator.generated.h"

USTRUCT(BlueprintType)
struct FDungeonGenerationData
{
	GENERATED_BODY()
public:

	FDungeonGenerationData() : NumberOfCrawlers(0), IterationMin(0), IterationMax(0) {}

	UPROPERTY(EditAnywhere, Category = "DungeonGenerationData")
		int32 NumberOfCrawlers;

	UPROPERTY(EditAnywhere, Category = "DungeonGenerationData")
		int32 IterationMin;

	UPROPERTY(EditAnywhere, Category = "DungeonGenerationData")
		int32 IterationMax;
};

UCLASS()
class ARENABATTLE_API ADungeonGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADungeonGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	/*UPROPERTY(VisibleAnywhere, Category = Section)
		class USectionController* SectionController;*/
	UPROPERTY()
	TArray<FIntVector> PositionVisited;

	TMap<int32, FIntVector> DirectionMovementMap;

	UPROPERTY(EditAnywhere, Category = DungeonGenerationData)
		int32 NumberOfCrawlers;

	UPROPERTY(EditAnywhere, Category = DungeonGenerationData)
		int32 IterationMin;

	UPROPERTY(EditAnywhere, Category = DungeonGenerationData)
		int32 IterationMax;

public:
	void InformOpenDoor(class AABSection* OppositeSection);
	void InformCloseDoor(class AABSection* Section);

	TMap<int, class AABSection*> GetABSections();
private:

	struct FDungeonGenerationData DungeonGenerationData;
	
	FIntVector Position;
	
	UPROPERTY(VisibleAnywhere, Category = Dungeon)
		TArray<FIntVector> DungeonSections;

	UPROPERTY(EditAnywhere, Category = Dungeon)
	TSubclassOf<class ASectionController> SectionControllerClass;

	UPROPERTY(EditAnywhere, Category = Box)
		TSubclassOf<class AABSection> ABSectionClass;

	UPROPERTY(VisibleAnywhere, Category = Dungeon)
	class ASectionController* SectionController;
	

	TMap<int, class AABSection*> ABSections;

private: 
	FIntVector Move(AABSection* ABSection);

	void SpawnSectioin(TArray<FIntVector> SectionsLocation);

	void CheckAroundSection(class AABSection* NextSection);

	AABSection* GetRandomSection();

	void UnConnectedDoors();


};
 