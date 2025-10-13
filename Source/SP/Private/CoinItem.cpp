#include "CoinItem.h"
#include "Engine/World.h"
#include "SpartaGameBase.h"

ACoinItem::ACoinItem()
{
	PointValue = 0;
	ItemType = "DefaultCoin";
}

void ACoinItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);
	
	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (UWorld* world = GetWorld())
		{
			if (ASpartaGameBase* ASGameState = world->GetGameState<ASpartaGameBase>())
			{
				ASGameState->AddScore(PointValue);
				ASGameState->OnCoinCollected();
			}
		}
		DestroyItem();
	}
}
