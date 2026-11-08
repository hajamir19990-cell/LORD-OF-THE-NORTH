#include "LOTNSaveSubsystem.h"
#include "LOTNSaveGame.h"
#include "LOTNPlayerStatsComponent.h"
#include "LOTNQuestComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

bool ULOTNSaveSubsystem::SaveCurrentGame()
{
    ACharacter* Player = UGameplayStatics::GetPlayerCharacter(this, 0);
    if (!Player)
    {
        return false;
    }

    ULOTNSaveGame* SaveObject = Cast<ULOTNSaveGame>(UGameplayStatics::CreateSaveGameObject(ULOTNSaveGame::StaticClass()));
    if (!SaveObject)
    {
        return false;
    }

    SaveObject->PlayerLocation = Player->GetActorLocation();
    SaveObject->PlayerRotation = Player->GetActorRotation();
    SaveObject->CurrentLevelName = FName(*UGameplayStatics::GetCurrentLevelName(this, true));

    if (const ULOTNPlayerStatsComponent* Stats = Player->FindComponentByClass<ULOTNPlayerStatsComponent>())
    {
        SaveObject->Health = Stats->Health;
        SaveObject->Stamina = Stats->Stamina;
        SaveObject->Gold = Stats->Gold;
        SaveObject->Renown = Stats->Renown;
    }

    if (const ULOTNQuestComponent* Quests = Player->FindComponentByClass<ULOTNQuestComponent>())
    {
        SaveObject->CompletedQuestIds = Quests->CompletedQuestIds;
        SaveObject->ActiveQuests = Quests->ActiveQuests;
    }

    return UGameplayStatics::SaveGameToSlot(SaveObject, SaveSlotName, 0);
}

bool ULOTNSaveSubsystem::LoadCurrentGame()
{
    if (!UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
    {
        return false;
    }

    ACharacter* Player = UGameplayStatics::GetPlayerCharacter(this, 0);
    if (!Player)
    {
        return false;
    }

    ULOTNSaveGame* SaveObject = Cast<ULOTNSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
    if (!SaveObject)
    {
        return false;
    }

    const FName CurrentLevel(*UGameplayStatics::GetCurrentLevelName(this, true));
    if (SaveObject->CurrentLevelName != NAME_None && SaveObject->CurrentLevelName != CurrentLevel)
    {
        return false;
    }

    Player->SetActorLocationAndRotation(SaveObject->PlayerLocation, SaveObject->PlayerRotation, false, nullptr, ETeleportType::TeleportPhysics);

    if (ULOTNPlayerStatsComponent* Stats = Player->FindComponentByClass<ULOTNPlayerStatsComponent>())
    {
        Stats->Health = FMath::Clamp(SaveObject->Health, 0.f, Stats->MaxHealth);
        Stats->Stamina = FMath::Clamp(SaveObject->Stamina, 0.f, Stats->MaxStamina);
        Stats->Gold = FMath::Max(0, SaveObject->Gold);
        Stats->Renown = FMath::Max(0, SaveObject->Renown);
        Stats->OnStatsChanged.Broadcast();
    }

    if (ULOTNQuestComponent* Quests = Player->FindComponentByClass<ULOTNQuestComponent>())
    {
        Quests->RestoreQuestState(SaveObject->CompletedQuestIds, SaveObject->ActiveQuests);
    }

    return true;
}
