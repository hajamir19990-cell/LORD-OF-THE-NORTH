#pragma once

#include "CoreMinimal.h"
#include "Engine/PrimaryDataAsset.h"
#include "LOTNWorldMapData.generated.h"

USTRUCT(BlueprintType)
struct FLOTNWorldLocation
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Location")
    FName LocationId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Location")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Location", meta=(MultiLine="true"))
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Location")
    FName LevelName = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Location", meta=(ClampMin="0.0", ClampMax="1.0"))
    FVector2D MapPosition = FVector2D(0.5f, 0.5f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Location")
    FName RegionType = FName(TEXT("Wilderness"));

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Location")
    TArray<FName> ConnectedLocationIds;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Location")
    bool bUnlockedByDefault = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Location", meta=(ClampMin="0"))
    int32 RequiredRenown = 0;
};

UCLASS(BlueprintType)
class LORDOFTHENORTH_API ULOTNWorldMapData : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    ULOTNWorldMapData();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="World Map")
    TArray<FLOTNWorldLocation> Locations;

    UFUNCTION(BlueprintCallable, Category="LOTN|World Map")
    bool GetLocationById(FName LocationId, FLOTNWorldLocation& OutLocation) const;

    UFUNCTION(BlueprintPure, Category="LOTN|World Map")
    bool IsLocationUnlocked(FName LocationId, int32 CurrentRenown) const;

    UFUNCTION(BlueprintPure, Category="LOTN|World Map")
    bool IsLocationReadyToTravel(FName LocationId, int32 CurrentRenown) const;
};
