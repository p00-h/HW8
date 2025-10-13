#include "SpawnVolume.h"

#include "Components/BoxComponent.h"

ASpawnVolume::ASpawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = Scene;

	SpawnningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Spawning Box"));
	SpawnningBox->SetupAttachment(Scene);

	ItemDataTable = nullptr;
}

void ASpawnVolume::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASpawnVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector ASpawnVolume::GetRandomSpawnPointVolume() const
{
	FVector BoxExtent = SpawnningBox->GetScaledBoxExtent();
	FVector BoxOrigin = SpawnningBox->GetComponentLocation();

	return BoxOrigin + FVector(
		FMath::FRandRange(-BoxExtent.X, BoxExtent.X),
		FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y),
		FMath::FRandRange(-BoxExtent.Z, BoxExtent.Z)
	);
}

AActor* ASpawnVolume::SpawnRandomItem()
{
	if (FItemSpawnRow* SelectedRow = GetRandomItem())
	{
		if (UClass* ActualCalss = SelectedRow->ItemClass.Get())
		{
			return SpawnItem(ActualCalss);
		}
	}
	return nullptr;
}

FItemSpawnRow* ASpawnVolume::GetRandomItem() const
{
	if (!ItemDataTable) return nullptr;

	TArray<FItemSpawnRow*> AllRows;
	static const FString ContextString(TEXT("Spawning Item DataTable"));
	ItemDataTable->GetAllRows(ContextString, AllRows);

	if (AllRows.IsEmpty()) return nullptr;

	float TotalChance = 0.0f;
	for (FItemSpawnRow* Row : AllRows)
	{
		if (Row)
		{
			TotalChance += Row->Spawnchance;
		}
	}

	const float RandValue = FMath::FRandRange(0, TotalChance);
	float AccumulateChance = 0.0f;
	for (FItemSpawnRow* Row : AllRows)
	{
		AccumulateChance += Row->Spawnchance;
		if (RandValue <= AccumulateChance)
		{
			return Row;
		}
	}
	return nullptr;
}

AActor* ASpawnVolume::SpawnItem(TSubclassOf<AActor> ItemClass)
{
	if (!ItemClass) return nullptr;

	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
		ItemClass,
		GetRandomSpawnPointVolume(),
		FRotator::ZeroRotator
	);

	return SpawnedActor;
}

