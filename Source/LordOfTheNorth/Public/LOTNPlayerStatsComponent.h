#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LOTNPlayerStatsComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLOTNStatsChanged);

UCLASS(ClassGroup=(LOTN), meta=(BlueprintSpawnableComponent))
class LORDOFTHENORTH_API ULOTNPlayerStatsComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    ULOTNPlayerStatsComponent();

    UPROPERTY(BlueprintAssignable, Category="LOTN|Stats")
    FLOTNStatsChanged OnStatsChanged;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="LOTN|Stats")
    float MaxHealth = 100.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="LOTN|Stats")
    float Health = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="LOTN|Stats")
    float MaxStamina = 100.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="LOTN|Stats")
    float Stamina = 100.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="LOTN|Progression")
    int32 Gold = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="LOTN|Progression")
    int32 Renown = 0;

    UFUNCTION(BlueprintCallable, Category="LOTN|Stats")
    void ApplyDamage(float Amount);

    UFUNCTION(BlueprintCallable, Category="LOTN|Stats")
    bool ConsumeStamina(float Amount);

    UFUNCTION(BlueprintCallable, Category="LOTN|Stats")
    void RestoreStamina(float Amount);

    UFUNCTION(BlueprintCallable, Category="LOTN|Progression")
    void AddGold(int32 Amount);

    UFUNCTION(BlueprintCallable, Category="LOTN|Progression")
    void AddRenown(int32 Amount);

    UFUNCTION(BlueprintCallable, Category="LOTN|Stats")
    void RestoreHealth(float Amount);
};
