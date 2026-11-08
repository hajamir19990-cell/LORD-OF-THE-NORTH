#include "LOTNPlayerStatsComponent.h"

ULOTNPlayerStatsComponent::ULOTNPlayerStatsComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void ULOTNPlayerStatsComponent::ApplyDamage(float Amount)
{
    if (Amount <= 0.f) return;
    Health = FMath::Clamp(Health - Amount, 0.f, FMath::Max(0.f, MaxHealth));
    OnStatsChanged.Broadcast();
}

bool ULOTNPlayerStatsComponent::ConsumeStamina(float Amount)
{
    if (Amount <= 0.f) return true;
    if (Stamina < Amount) return false;
    Stamina = FMath::Clamp(Stamina - Amount, 0.f, FMath::Max(0.f, MaxStamina));
    OnStatsChanged.Broadcast();
    return true;
}

void ULOTNPlayerStatsComponent::RestoreStamina(float Amount)
{
    if (Amount <= 0.f) return;
    Stamina = FMath::Clamp(Stamina + Amount, 0.f, FMath::Max(0.f, MaxStamina));
    OnStatsChanged.Broadcast();
}

void ULOTNPlayerStatsComponent::AddGold(int32 Amount)
{
    Gold = static_cast<int32>(FMath::Clamp<int64>(static_cast<int64>(Gold) + Amount, 0, MAX_int32));
    OnStatsChanged.Broadcast();
}

void ULOTNPlayerStatsComponent::AddRenown(int32 Amount)
{
    Renown = static_cast<int32>(FMath::Clamp<int64>(static_cast<int64>(Renown) + Amount, 0, MAX_int32));
    OnStatsChanged.Broadcast();
}

void ULOTNPlayerStatsComponent::RestoreHealth(float Amount)
{
    if (Amount <= 0.f) return;
    Health = FMath::Clamp(Health + Amount, 0.f, FMath::Max(0.f, MaxHealth));
    OnStatsChanged.Broadcast();
}
