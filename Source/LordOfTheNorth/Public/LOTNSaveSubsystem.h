#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LOTNSaveSubsystem.generated.h"

UCLASS()
class LORDOFTHENORTH_API ULOTNSaveSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="LOTN|Save")
    bool SaveCurrentGame();

    UFUNCTION(BlueprintCallable, Category="LOTN|Save")
    bool LoadCurrentGame();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="LOTN|Save")
    FString SaveSlotName = TEXT("LOTN_Save_01");
};
