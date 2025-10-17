// Fill out your copyright notice in the Description page of Project Settings.


#include "SpartaGameBase.h"

#include "CoinItem.h"
#include "SpartaCharacter.h"
#include "SpartaGameInstance.h"
#include "SpartaPlayerController.h"
#include "SpawnVolume.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

ASpartaGameBase::ASpartaGameBase()
{
	Score = 0;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	LevelDuration = 20.0f;
	CurrentLevelIndex = 0;
	MaxLevels = 3;

	CurrentWaveIndex = 0;
	MaxWaveCount = 3;
	WaveDuration = 20.0f;
}

void ASpartaGameBase::BeginPlay()
{
	Super::BeginPlay();

	StartLevel();
	
	GetWorldTimerManager().SetTimer(
		HUDUpdateTimerHandle,
		this,
		&ASpartaGameBase::UpdateHUD,
		0.1f,
		true
	);
	
}

int32 ASpartaGameBase::GetScore() const
{
	return Score;
}

void ASpartaGameBase::AddScore(int32 Amount)
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			SpartaGameInstance->AddToScore(Amount);
		}
	}
}


void ASpartaGameBase::OnCoinCollected()
{
	++CollectedCoinCount;
	++WaveCollectedCoinCount;

	UE_LOG(LogTemp, Warning, TEXT("Wave %d Coin: %d / %d"),
	       CurrentWaveIndex, WaveCollectedCoinCount, WaveSpawnedCoinCount);

	if (WaveSpawnedCoinCount > 0 && WaveCollectedCoinCount >= WaveSpawnedCoinCount)
	{
		// 웨이브 목표 달성 → 웨이브 타이머 정리 후 다음 단계로
		GetWorldTimerManager().ClearTimer(WaveTimerHandle);

		const bool bLastWave = (CurrentWaveIndex >= MaxWaveCount);
		if (bLastWave)
		{
			OnLevelTimeUp();  // 마지막 웨이브면 레벨 종료
		}
		else
		{
			OnWaveTimeUp();   // 다음 웨이브로
		}
	}
}

void ASpartaGameBase::StartLevel()
{

	CurrentWaveIndex = 0;
	StartWave();

	
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->ShowGameHUD();
		}
	}
	
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			CurrentLevelIndex = SpartaGameInstance->CurrentLevelIndex;
		}
	}
	
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;

	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);
	
	UpdateHUD();

	UE_LOG(LogTemp, Warning, TEXT("Level %d Start!, Spawned %d coin"),
	       CurrentLevelIndex + 1,
	       SpawnedCoinCount);
	
}

void ASpartaGameBase::EndLevel()
{
	GetWorldTimerManager().ClearTimer(LevelTimerHandle);
	
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			AddScore(Score);
			CurrentLevelIndex++;
			SpartaGameInstance->CurrentLevelIndex = CurrentLevelIndex;
		}
	}
	
	if (CurrentLevelIndex >= MaxLevels)
	{
		OnGameOver();
		return;
	}

	if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
	{
		UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
	}
	else
	{
		OnGameOver();
	}
}

void ASpartaGameBase::OnLevelTimeUp()
{
	EndLevel();
}

void ASpartaGameBase::OnGameOver()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->SetPause(true);
			SpartaPlayerController->ShowMainMenu(true);
		}
	}
}

void ASpartaGameBase::UpdateHUD()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if(ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			if (UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
			{
				if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time"))))
				{
					float RemainingTime = GetWorldTimerManager().GetTimerRemaining(WaveTimerHandle);
					if (RemainingTime < 0.f) RemainingTime = 0.f;
					TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time: %.1f"), RemainingTime)));
				}
				
				if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
				{
					if (UGameInstance* GameInstance = GetGameInstance())
					{
						USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
						if (SpartaGameInstance)
						{
							ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), SpartaGameInstance->TotalScore)));
						}
					}
				}

				if (UTextBlock* WaveIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Wave"))))
				{
					WaveIndexText->SetText(FText::FromString(FString::Printf(TEXT("Wave: %d"), CurrentWaveIndex)));
				}
				
				if (UTextBlock* LevelIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Level"))))
				{
					LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("Level: %d"), CurrentLevelIndex + 1)));
				}

				if (UProgressBar* HP = Cast<UProgressBar>(HUDWidget->GetWidgetFromName(TEXT("HP"))))
				{
					if (APawn* Pawn = SpartaPlayerController->GetPawn())
					{
						if (ASpartaCharacter* MyChar = Cast<ASpartaCharacter>(Pawn))
						{
							const float Ratio = (MyChar->GetMaxHealth() > 0.f) ? (MyChar->GetHealth() / MyChar->GetMaxHealth()) : 0.f;
							HP->SetPercent(FMath::Clamp(Ratio, 0.f, 1.f));
						}
					}
				}
			}
		}
	}
}

void ASpartaGameBase::StartWave()
{
	GetWorldTimerManager().ClearTimer(WaveTimerHandle);
	GetWorldTimerManager().ClearTimer(LevelTimerHandle);

	CurrentWaveIndex++;
	WaveSpawnedCoinCount = 0;
	WaveCollectedCoinCount = 0;

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan,
		                                 FString::Printf(TEXT("Wave %d 시작!"), CurrentWaveIndex));

	const int32 ItemsToSpawn = 20 + (CurrentWaveIndex - 1) * 10; // 예시
	SpawnItems(ItemsToSpawn); // 아래 4)에서 카운트 갱신 추가

	const bool bLastWave = (CurrentWaveIndex >= MaxWaveCount);
	GetWorldTimerManager().SetTimer(
		/*Out*/ WaveTimerHandle,
		        this,
		        bLastWave ? &ASpartaGameBase::OnLevelTimeUp
			        : &ASpartaGameBase::OnWaveTimeUp,
		        WaveDuration,
		        false
	);
}

void ASpartaGameBase::OnWaveTimeUp()
{
	EndWave();
}

void ASpartaGameBase::EndWave()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow,
	                                 FString::Printf(TEXT("Wave %d 종료!"), CurrentWaveIndex));

	if (CurrentWaveIndex < MaxWaveCount)
	{
		StartWave();
	}
	else
	{
		OnLevelTimeUp(); // 모든 웨이브 끝나면 레벨 종료
	}
}

void ASpartaGameBase::SpawnItems(int32 Count)
{
	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);
	if (FoundVolumes.IsEmpty()) return;

	ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
	if (!SpawnVolume) return;

	for (int32 i = 0; i < Count; ++i)
	{
		if (AActor* SpawnedActor = SpawnVolume->SpawnRandomItem())
		{
			if (SpawnedActor->IsA(ACoinItem::StaticClass()))
			{
				// 레벨 전체 카운트
				++SpawnedCoinCount;
				// 웨이브 카운트
				++WaveSpawnedCoinCount;
			}
		}
	}
}
