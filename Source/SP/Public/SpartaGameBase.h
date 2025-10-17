#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "SpartaGameBase.generated.h"


UCLASS()
class SP_API ASpartaGameBase : public AGameState
{
	GENERATED_BODY()

public:
	ASpartaGameBase();

	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite,Category = "Score")
	int32 Score;

	UFUNCTION(BlueprintPure, Category = "Score")
	int32 GetScore() const;
	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddScore(int32 Amount);
	
public:
	void OnCoinCollected();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Coin")
	int32 SpawnedCoinCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Coin")
	int32 CollectedCoinCount;

public:
	void StartLevel();
	void EndLevel();
	
	FTimerHandle LevelTimerHandle;
	void OnLevelTimeUp();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = "Level")
	float LevelDuration;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Level")
	int32 CurrentLevelIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Level")
	int32 MaxLevels;
	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = "Level")
	TArray<FName> LevelMapNames;

	UFUNCTION(BlueprintCallable, Category = "Level")
	void OnGameOver();
	
public:
	FTimerHandle HUDUpdateTimerHandle;
	void UpdateHUD();

public:
	void StartWave();
	void OnWaveTimeUp();
	void EndWave();
	void SpawnItems(int32 Count);
	FTimerHandle WaveTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
	int32 CurrentWaveIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	int32 MaxWaveCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	float WaveDuration;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Wave")
	int32 WaveSpawnedCoinCount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Wave")
	int32 WaveCollectedCoinCount = 0;
};
