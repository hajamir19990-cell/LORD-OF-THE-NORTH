#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "LOTNSaveGame.generated.h"

UCLASS()
class LORDOFTHENORTH_API ULOTNSaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    UPROPERTY(VisibleAnywhere, Category="Save")
    int32 SaveVersion = 1;

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
    FName CurrentLocationId = TEXT("Winterfell");

    UPROPERTY(VisibleAnywhere, Category="World")
    TArray<FName> CompletedQuestIds;
};
