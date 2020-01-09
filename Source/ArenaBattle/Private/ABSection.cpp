// Fill out your copyright notice in the Description page of Project Settings.

#include "ABSection.h"
#include "ABCharacter.h"
#include "ABItemBox.h"
#include "ABPlayerController.h"
#include "ABGameMode.h"
#include "DungeonGenerator.h"
#include "GamePlayMiniMap.h"
#include "SectionController.h"


// Sets default values
AABSection::AABSection()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH"));
	RootComponent = Mesh;

	FString AssetPath = TEXT("/Game/Book/StaticMesh/SM_SQUARE.SM_SQUARE");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_SQUARE(*AssetPath);
	if (SM_SQUARE.Succeeded())
	{
		Mesh->SetStaticMesh(SM_SQUARE.Object);
	}
	else
	{
		ABLOG(Error, TEXT("Failed to load staticmesh asset. : %s"), *AssetPath);
	}

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TRIGGER"));
	Trigger->SetBoxExtent(FVector(775.0f, 775.0f, 300.0f));
	Trigger->SetupAttachment(RootComponent);
	Trigger->SetRelativeLocation(FVector(0.0f, 0.0f, 250.0f));
	Trigger->SetCollisionProfileName(TEXT("ABTrigger"));

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AABSection::OnTriggerBeginOverlap);

	FString GateAssetPath = TEXT("/Game/Book/StaticMesh/SM_GATE.SM_GATE");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_GATE(*GateAssetPath);
	if (!SM_GATE.Succeeded())
	{
		ABLOG(Error, TEXT("Failed to load staticmesh asset. : %s"), *GateAssetPath);
	}

	FString WallAssetPath = TEXT("/Game/Book/StaticMesh/Wall.Wall");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_WALL(*WallAssetPath);
	if (SM_WALL.Succeeded())
	{
		WallMesh = SM_WALL.Object;
	}

	static FName GateSockets[] = { {TEXT("+XGate")}, {TEXT("-XGate")}, {TEXT("+YGate")}, {TEXT("-YGate")} };
	for (FName GateSocket : GateSockets)
	{
		ABCHECK(Mesh->DoesSocketExist(GateSocket));
		UStaticMeshComponent* NewGate = CreateDefaultSubobject<UStaticMeshComponent>(*GateSocket.ToString());
		NewGate->SetStaticMesh(SM_GATE.Object);
		NewGate->SetupAttachment(RootComponent, GateSocket);
		NewGate->SetRelativeLocation(FVector(0.0f, -80.5f, 0.0f));
		GateMeshes.Add(NewGate);

		UBoxComponent* NewGateTrigger = CreateDefaultSubobject<UBoxComponent>(*GateSocket.ToString().Append(TEXT("Trigger")));
		NewGateTrigger->SetBoxExtent(FVector(100.0f, 100.0f, 300.0f));
		NewGateTrigger->SetupAttachment(RootComponent, GateSocket);
		NewGateTrigger->SetRelativeLocation(FVector(70.0f, 0.0f, 250.0f));
		NewGateTrigger->SetCollisionProfileName(TEXT("ABTrigger"));
		GateTriggers.Add(NewGateTrigger);

		NewGateTrigger->OnComponentBeginOverlap.AddDynamic(this, &AABSection::OnGateTriggerBeginOverlap);
		NewGateTrigger->ComponentTags.Add(GateSocket);
	}

	bNoBattle = false;

	EnemySpawnTime = 2.0f;
	ItemBoxSpawnTime = 2.0f;

	for (int32 i = 0; i < 4; i++)
	{
		ConnectedDoors.Add(i);
		bConnectedDoors.Add(false);
	}
}

void AABSection::OnConstruction(const FTransform & Transform)
{
	Super::OnConstruction(Transform);
	//SetState(bNoBattle ? ESectionState::COMPLETE : ESectionState::READY);
}

// Called when the game starts or when spawned
void AABSection::BeginPlay()
{
	Super::BeginPlay();

	//SetState(bNoBattle ? ESectionState::COMPLETE : ESectionState::READY);
}

void AABSection::SetState(ESectionState NewState)
{
	switch (NewState)
	{
	case ESectionState::READY:
	{
		Trigger->SetCollisionProfileName(TEXT("ABTrigger"));
		for (UBoxComponent* GateTrigger : GateTriggers)
		{
			GateTrigger->SetCollisionProfileName(TEXT("NoCollision"));
		}

		OperateGates(true);
		break;
	}
	case ESectionState::BATTLE:
	{
		Trigger->SetCollisionProfileName(TEXT("NoCollision"));
		for (UBoxComponent* GateTrigger : GateTriggers)
		{
			GateTrigger->SetCollisionProfileName(TEXT("NoCollision"));
		}

		OperateGates(false);

		GetWorld()->GetTimerManager().SetTimer(SpawnNPCTimerHandle, FTimerDelegate::CreateUObject(this, &AABSection::OnNPCSpawn), EnemySpawnTime, false);

		GetWorld()->GetTimerManager().SetTimer(SpawnItemBoxTimerHandle, FTimerDelegate::CreateLambda([this]() -> void {
			ABLOG(Warning, TEXT("SpawnItem."));
			FVector2D RandXY = FMath::RandPointInCircle(600.0f);
			GetWorld()->SpawnActor<AABItemBox>(GetActorLocation() + FVector(RandXY, 30.0f), FRotator::ZeroRotator);

		}), ItemBoxSpawnTime, false);

		break;
	}
	case ESectionState::COMPLETE:
	{
		Trigger->SetCollisionProfileName(TEXT("ABTrigger"));
		for (UBoxComponent* GateTrigger : GateTriggers)
		{
			GateTrigger->SetCollisionProfileName(TEXT("ABTrigger"));
		}

		OperateGates(true);
		DungeonGenerator->InformOpenDoor(this);

		break;
	}
	}

	CurrentState = NewState;
}

void AABSection::OperateGates(bool bOpen)
{
	for (UStaticMeshComponent* Gate : GateMeshes)
	{
		if (Gate->GetStaticMesh() != WallMesh)
		{
			Gate->SetRelativeRotation(bOpen ? FRotator(0.0f, -90.0f, 0.0f) : FRotator::ZeroRotator);
		}
	}
}

void AABSection::OnTriggerBeginOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (CurrentState == ESectionState::READY)
	{
		SetState(ESectionState::BATTLE);		
	}

	auto PlayerController = Cast<AABPlayerController>(GetWorld()->GetFirstPlayerController());
	if (nullptr != PlayerController)
	{
		int32 index = FCString::Atoi(*GetRoomInfo()->Name);
		PlayerController->GetGamePlayMiniMap()->CurrentImageIndex(index);
	}
}

void AABSection::OnGateTriggerBeginOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	ABCHECK(OverlappedComponent->ComponentTags.Num() == 1);

}

void AABSection::OnNPCSpawn()
{
	auto KeyNPC = GetWorld()->SpawnActor<AABCharacter>(GetActorLocation() + FVector::UpVector * 88.0f, FRotator::ZeroRotator);
	if (nullptr != KeyNPC)
	{
		KeyNPC->OnDestroyed.AddDynamic(this, &AABSection::OnKeyNPCDestroyed);
	}
}

void AABSection::OnKeyNPCDestroyed(AActor * DestroyedActor)
{
	//방 Oepn 조건 확인. 하나만 죽으면 Open 발생 확인 조정.
	//방들어 갔을때 문 닫힘 확인.
	auto ABCharacter = Cast<AABCharacter>(DestroyedActor);
	ABCHECK(nullptr != ABCharacter);

	auto ABPlayerController = Cast<AABPlayerController>(ABCharacter->LastHitBy);
	ABCHECK(nullptr != ABPlayerController);

	auto ABGameMode = Cast<AABGameMode>(GetWorld()->GetAuthGameMode());
	ABCHECK(nullptr != ABGameMode);
	ABGameMode->AddScore(ABPlayerController);

	SetState(ESectionState::COMPLETE);
}

void AABSection::SetbConnectedDoors(FIntVector Direction)
{
	// Top = 0, Down = 1, Right = 2, Left = 3
	if (Direction == FIntVector(1, 0, 0))
	{
		bConnectedDoors[0] = true;
		for (int32 i = 0; i < ConnectedDoors.Num(); i++)
		{
			if (ConnectedDoors[i] == 0)
			{
				ConnectedDoors.RemoveAt(i);
			}
		}
	}
	else if (Direction == FIntVector(-1, 0, 0))
	{
		bConnectedDoors[1] = true;
		for (int32 i = 0; i < ConnectedDoors.Num(); i++)
		{
			if (ConnectedDoors[i] == 1)
			{
				ConnectedDoors.RemoveAt(i);
			}
		}
	}
	else if (Direction == FIntVector(0, 1, 0))
	{
		bConnectedDoors[2] = true;
		for (int32 i = 0; i < ConnectedDoors.Num(); i++)
		{
			if (ConnectedDoors[i] == 2)
			{
				ConnectedDoors.RemoveAt(i);
			}
		}
	}
	else if (Direction == FIntVector(0, -1, 0))
	{
		bConnectedDoors[3] = true;
		for (int32 i = 0; i < ConnectedDoors.Num(); i++)
		{
			if (ConnectedDoors[i] == 3)
			{
				ConnectedDoors.RemoveAt(i);
			}
		}	
	}
}

void AABSection::SetUnConnectedDoors(FIntVector Direction)
{
	if (Direction == FIntVector(1, 0, 0))
	{
		for (int32 i = 0; i < ConnectedDoors.Num(); i++)
		{
			if (ConnectedDoors[i] == 0)
			{
				ConnectedDoors.RemoveAt(i);
			}
		}
	}
	else if (Direction == FIntVector(-1, 0, 0))
	{
		for (int32 i = 0; i < ConnectedDoors.Num(); i++)
		{
			if (ConnectedDoors[i] == 1)
			{
				ConnectedDoors.RemoveAt(i);
			}
		}
	}
	else if (Direction == FIntVector(0, 1, 0))
	{
		for (int32 i = 0; i < ConnectedDoors.Num(); i++)
		{
			if (ConnectedDoors[i] == 2)
			{
				ConnectedDoors.RemoveAt(i);
			}
		}
	}
	else if (Direction == FIntVector(0, -1, 0))
	{
		for (int32 i = 0; i < ConnectedDoors.Num(); i++)
		{
			if (ConnectedDoors[i] == 3)
			{
				ConnectedDoors.RemoveAt(i);
			}
		}
	}
}

void AABSection::OpenGate(FIntVector Direction, bool bOpen)
{
	if (Direction == FIntVector(1, 0, 0))
	{	
		if (GateMeshes[0]->GetStaticMesh() != WallMesh)
			GateMeshes[0]->SetRelativeRotation(bOpen ? FRotator(0.0f, -90.0f, 0.0f) : FRotator::ZeroRotator);
	}
	else if (Direction == FIntVector(-1, 0, 0))
	{
		if (GateMeshes[1]->GetStaticMesh() != WallMesh)
			GateMeshes[1]->SetRelativeRotation(bOpen ? FRotator(0.0f, -90.0f, 0.0f) : FRotator::ZeroRotator);
	}
	else if (Direction == FIntVector(0, 1, 0))
	{
		if (GateMeshes[2]->GetStaticMesh() != WallMesh)
			GateMeshes[2]->SetRelativeRotation(bOpen ? FRotator(0.0f, -90.0f, 0.0f) : FRotator::ZeroRotator);
	}
	else
	{
		if (GateMeshes[3]->GetStaticMesh() != WallMesh)
			GateMeshes[3]->SetRelativeRotation(bOpen ? FRotator(0.0f, -90.0f, 0.0f) : FRotator::ZeroRotator);
	}
}

FRoomInfo * AABSection::GetRoomInfo()
{
	return RoomInfo;
}

void AABSection::SetRoomInfo(FRoomInfo* NewRoomInfo)
{
	RoomInfo = NewRoomInfo;
}

FVector AABSection::GetRoomSize()
{
	return Mesh->CalcBounds(Mesh->GetComponentTransform()).BoxExtent * 2.0f;
}

void AABSection::SetDungeonGenerator(ADungeonGenerator * _DungeonGenerator)
{
	DungeonGenerator = _DungeonGenerator;
}

void AABSection::UnConnectedDoors()
{
	for (auto GateMesh : GateMeshes)
	{
		FString Name = GateMesh->GetName();
		if (Name == "+XGate")
		{
			if (GetbConnectedDoors()[0] == false)
			{
				GateMeshes[0]->SetStaticMesh(WallMesh);
				GateMeshes[0]->SetRelativeRotation(FRotator(0.0f, -180.0f, 0.0f));
				GateMeshes[0]->SetRelativeScale3D(FVector(0.45f, 0.45f, 0.45f));
				GateMeshes[0]->GetStaticMesh()->Rename(TEXT("Wall"));
			}
		}
		else if (Name == "-XGate")
		{
			if (GetbConnectedDoors()[1] == false)
			{
				GateMeshes[1]->SetStaticMesh(WallMesh);
				GateMeshes[1]->SetRelativeRotation(FRotator(0.0f, -180.0f, 0.0f));
				GateMeshes[1]->SetRelativeScale3D(FVector(0.45f, 0.45f, 0.45f));
				GateMeshes[1]->GetStaticMesh()->Rename(TEXT("Wall"));
			}
		}
		else if (Name == "+YGate")
		{
			if (GetbConnectedDoors()[2] == false)
			{
				GateMeshes[2]->SetStaticMesh(WallMesh);
				GateMeshes[2]->SetRelativeRotation(FRotator(0.0f, -180.0f, 0.0f));
				GateMeshes[2]->SetRelativeScale3D(FVector(0.45f, 0.45f, 0.45f));
				GateMeshes[2]->GetStaticMesh()->Rename(TEXT("Wall"));
			}
		}
		else
		{
			if (GetbConnectedDoors()[3] == false)
			{
				GateMeshes[3]->SetStaticMesh(WallMesh);
				GateMeshes[3]->SetRelativeRotation(FRotator(0.0f, -180.0f, 0.0f));
				GateMeshes[3]->SetRelativeScale3D(FVector(0.45f, 0.45f, 0.45f));
				GateMeshes[3]->GetStaticMesh()->Rename(TEXT("Wall"));
			}
		}
	}
}
