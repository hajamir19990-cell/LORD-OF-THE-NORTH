#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "LOTNQuestComponent.h"
#include "LOTNInventoryComponent.h"
#include "LOTNSaveGame.generated.h"

UCLASS()
class LORDOFTHENORTH_API ULOTNSaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    UPROPERTY(VisibleAnywhere, Category="Save")
    int32 SaveVersion = 3;

    UPROPERTY(VisibleAnywhere, Category="Player")
    FVector PlayerLocation = FVector::ZeroVector;

    UPROPERTY(VisibleAnywhere, Category="Player")
    FRotator PlayerRotation = FRotator::ZeroRotator;

    UPROPERTY(VisibleAnywhere, Category="Player")
    float Health = 100.f;

    UPROPERTY(VisibleAnywhere, Category="Player")
    float Stamina = 100.f;

    UPROPERTY(VisibleAnywhere, Category="Progression")
    int32 Gold = 0;

    UPROPERTY(VisibleAnywhere, Category="Progression")
    int32 Renown = 0;

    UPROPERTY(VisibleAnywhere, Category="World")
    FName CurrentLevelName = NAME_None;

    UPROPERTY(VisibleAnywhere, Category="World")
    FName CurrentLocationId = TEXT("Winterfell");

    UPROPERTY(VisibleAnywhere, Category="Quest")
    TArray<FName> CompletedQuestIds;

    UPROPERTY(VisibleAnywhere, Category="Quest")
    TArray<FLOTNQuestEntry> ActiveQuests;

    UPROPERTY(VisibleAnywhere, Category="Inventory")
    TArray<FLOTNInventoryItem> InventoryItems;

    UPROPERTY(VisibleAnywhere, Category="Inventory")
    TArray<FName> EquippedItemIds;
};
