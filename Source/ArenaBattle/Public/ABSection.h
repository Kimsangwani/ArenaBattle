// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/Actor.h"
#include "ABSection.generated.h"

UCLASS()
class ARENABATTLE_API AABSection : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AABSection();
	virtual void OnConstruction(const FTransform& Transform) override;
	enum class ESectionState : uint8
	{
		READY = 0,
		BATTLE,
		COMPLETE
	};
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
private:


	ESectionState CurrentState = ESectionState::READY;

	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnGateTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void OnNPCSpawn();

	UFUNCTION()
		void OnKeyNPCDestroyed(AActor* DestroyedActor);

	void OperateGates(bool bOpen = true);

private:
	UPROPERTY(VisibleAnywhere, Category = Mesh, Meta = (AllowPrivateAccess = true))
	TArray<UStaticMeshComponent*> GateMeshes;

	UPROPERTY(VisibleAnywhere, Category = Trigger, Meta = (AllowPrivateAccess = true))
	TArray<UBoxComponent*> GateTriggers;

	UPROPERTY(VisibleAnywhere, Category = Mesh, Meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, Category = Trigger, Meta = (AllowPrivateAccess = true))
	UBoxComponent* Trigger;

	UPROPERTY(EditAnywhere, Category = State, Meta = (AllowPrivateAccess = true))
	bool bNoBattle;

	UPROPERTY(EditAnywhere, Category = Sapwn, Meta = (AllowPrivateAccess = true))
		float EnemySpawnTime;

	UPROPERTY(EditAnywhere, Category = Sapwn, Meta = (AllowPrivateAccess = true))
		float ItemBoxSpawnTime;

	UPROPERTY()
		UStaticMesh* WallMesh;

	FTimerHandle SpawnNPCTimerHandle = { };
	FTimerHandle SpawnItemBoxTimerHandle = { };

	UPROPERTY()
		class ADungeonGenerator* DungeonGenerator;

	bool bConnectedTop;
	bool bConnectedDown;
	bool bConnectedLeft;
	bool bConnectedRight;

public:
	struct FRoomInfo* RoomInfo = nullptr;

	UPROPERTY()
	TArray<int> ConnectedDoors;

	UPROPERTY()
		TArray<bool> bConnectedDoors;

	void SetbConnectedDoors(FIntVector Direction);
	void SetUnConnectedDoors(FIntVector Direction);
	TArray<int> GetConnectedDoors() { return ConnectedDoors; }
	TArray<bool> GetbConnectedDoors() { return bConnectedDoors; }

	void OpenGate(FIntVector Direction, bool bOpen);

	void SetState(ESectionState NewState);

public:
	FRoomInfo* GetRoomInfo();
	void SetRoomInfo(FRoomInfo* NewRoomInfo);
	FVector GetRoomSize();

	void SetDungeonGenerator(class ADungeonGenerator* _DungeonGenerator);

	void UnConnectedDoors();
};
