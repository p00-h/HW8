#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SpartaGameInstance.generated.h"

UCLASS()
class SP_API USpartaGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	USpartaGameInstance();
	
	UPROPERTY(visibleAnywhere, BlueprintReadWrite,Category="GameData")
	int32 TotalScore;
	UPROPERTY(visibleAnywhere, BlueprintReadWrite,Category="GameData")
	int32 CurrentLevelIndex;

	UFUNCTION(BlueprintCallable, Category = "GameData")
	void AddToScore(int32 Amount);
};
