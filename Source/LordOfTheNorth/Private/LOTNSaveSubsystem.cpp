#include "LOTNSaveSubsystem.h"
#include "LOTNSaveGame.h"
#include "LOTNCharacter.h"
#include "LOTNPlayerStatsComponent.h"
#include "LOTNQuestComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

bool ULOTNSaveSubsystem::SaveCurrentGame()
{
    ALOTNCharacter* Player = Cast<ALOTNCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
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
    SaveObject->CurrentLocationId = Player->CurrentLocationId;

    if (const ULOTNPlayerStatsComponent* Stats = Player->PlayerStats)
    {
        SaveObject->Health = Stats->Health;
        SaveObject->Stamina = Stats->Stamina;
        SaveObject->Gold = Stats->Gold;
        SaveObject->Renown = Stats->Renown;
    }

    if (const ULOTNQuestComponent* Quests = Player->QuestComponent)
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

    ALOTNCharacter* Player = Cast<ALOTNCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
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
    Player->CurrentLocationId = SaveObject->CurrentLocationId;

    if (ULOTNPlayerStatsComponent* Stats = Player->PlayerStats)
    {
        Stats->Health = FMath::Clamp(SaveObject->Health, 0.f, Stats->MaxHealth);
        Stats->Stamina = FMath::Clamp(SaveObject->Stamina, 0.f, Stats->MaxStamina);
        Stats->Gold = FMath::Max(0, SaveObject->Gold);
        Stats->Renown = FMath::Max(0, SaveObject->Renown);
        Stats->OnStatsChanged.Broadcast();
    }

    if (ULOTNQuestComponent* Quests = Player->QuestComponent)
    {
        Quests->RestoreQuestState(SaveObject->CompletedQuestIds, SaveObject->ActiveQuests);
    }

    return true;
}
