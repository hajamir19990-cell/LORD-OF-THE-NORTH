#include "LOTNHUDWidget.h"
#include "LOTNCharacter.h"
#include "LOTNPlayerStatsComponent.h"
#include "LOTNQuestComponent.h"
#include "LOTNInventoryComponent.h"
#include "LOTNInteractionComponent.h"
#include "LOTNSaveSubsystem.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"

void ULOTNHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();
    BindPlayerEvents(true);

    if (InteractButton)
    {
        InteractButton->OnClicked.AddDynamic(this, &ULOTNHUDWidget::HandleInteractClicked);
    }
    if (SaveButton)
    {
        SaveButton->OnClicked.AddDynamic(this, &ULOTNHUDWidget::HandleSaveClicked);
    }
    if (LoadButton)
    {
        LoadButton->OnClicked.AddDynamic(this, &ULOTNHUDWidget::HandleLoadClicked);
    }

    RefreshHUD();
}

void ULOTNHUDWidget::NativeDestruct()
{
    BindPlayerEvents(false);

    if (InteractButton)
    {
        InteractButton->OnClicked.RemoveDynamic(this, &ULOTNHUDWidget::HandleInteractClicked);
    }
    if (SaveButton)
    {
        SaveButton->OnClicked.RemoveDynamic(this, &ULOTNHUDWidget::HandleSaveClicked);
    }
    if (LoadButton)
    {
        LoadButton->OnClicked.RemoveDynamic(this, &ULOTNHUDWidget::HandleLoadClicked);
    }

    Super::NativeDestruct();
}

void ULOTNHUDWidget::BindPlayerEvents(bool bBind)
{
    ALOTNCharacter* Player = Cast<ALOTNCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    if (!Player)
    {
        return;
    }

    if (Player->PlayerStats)
    {
        if (bBind)
        {
            Player->PlayerStats->OnStatsChanged.AddDynamic(this, &ULOTNHUDWidget::HandleStatsChanged);
        }
        else
        {
            Player->PlayerStats->OnStatsChanged.RemoveDynamic(this, &ULOTNHUDWidget::HandleStatsChanged);
        }
    }

    if (Player->QuestComponent)
    {
        if (bBind)
        {
            Player->QuestComponent->OnQuestStateChanged.AddDynamic(this, &ULOTNHUDWidget::HandleQuestChanged);
        }
        else
        {
            Player->QuestComponent->OnQuestStateChanged.RemoveDynamic(this, &ULOTNHUDWidget::HandleQuestChanged);
        }
    }

    if (Player->InventoryComponent)
    {
        if (bBind)
        {
            Player->InventoryComponent->OnInventoryChanged.AddDynamic(this, &ULOTNHUDWidget::HandleInventoryChanged);
        }
        else
        {
            Player->InventoryComponent->OnInventoryChanged.RemoveDynamic(this, &ULOTNHUDWidget::HandleInventoryChanged);
        }
    }

    if (Player->InteractionComponent)
    {
        if (bBind)
        {
            Player->InteractionComponent->OnInteractionMessage.AddDynamic(this, &ULOTNHUDWidget::HandleInteractionMessage);
        }
        else
        {
            Player->InteractionComponent->OnInteractionMessage.RemoveDynamic(this, &ULOTNHUDWidget::HandleInteractionMessage);
        }
    }
}

void ULOTNHUDWidget::RefreshHUD()
{
    ALOTNCharacter* Player = Cast<ALOTNCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    if (!Player)
    {
        return;
    }

    if (const ULOTNPlayerStatsComponent* Stats = Player->PlayerStats)
    {
        if (HealthBar)
        {
            HealthBar->SetPercent(Stats->MaxHealth > 0.f ? Stats->Health / Stats->MaxHealth : 0.f);
        }
        if (StaminaBar)
        {
            StaminaBar->SetPercent(Stats->MaxStamina > 0.f ? Stats->Stamina / Stats->MaxStamina : 0.f);
        }
        if (GoldText)
        {
            GoldText->SetText(FText::AsNumber(Stats->Gold));
        }
        if (RenownText)
        {
            RenownText->SetText(FText::AsNumber(Stats->Renown));
        }
    }

    if (InventoryCountText && Player->InventoryComponent)
    {
        InventoryCountText->SetText(FText::AsNumber(Player->InventoryComponent->GetTotalItemCount()));
    }

    if (QuestTitleText && QuestObjectiveText && QuestProgressText)
    {
        if (Player->QuestComponent && Player->QuestComponent->ActiveQuests.Num() > 0)
        {
            const FLOTNQuestEntry& Quest = Player->QuestComponent->ActiveQuests[0];
            QuestTitleText->SetText(Quest.Title);
            QuestObjectiveText->SetText(Quest.ObjectiveText);
            QuestProgressText->SetText(FText::Format(
                NSLOCTEXT("LOTN", "QuestProgressFormat", "{0} / {1}"),
                FText::AsNumber(Quest.CurrentProgress),
                FText::AsNumber(Quest.RequiredProgress)));
        }
        else
        {
            QuestTitleText->SetText(NSLOCTEXT("LOTN", "NoActiveQuest", "No active quest"));
            QuestObjectiveText->SetText(FText::GetEmpty());
            QuestProgressText->SetText(FText::GetEmpty());
        }
    }
}

void ULOTNHUDWidget::ShowDialogue(const FText& Message)
{
    if (DialogueText)
    {
        DialogueText->SetText(Message);
        DialogueText->SetVisibility(ESlateVisibility::Visible);
    }
}

void ULOTNHUDWidget::HandleStatsChanged()
{
    RefreshHUD();
}

void ULOTNHUDWidget::HandleQuestChanged()
{
    RefreshHUD();
}

void ULOTNHUDWidget::HandleInventoryChanged()
{
    RefreshHUD();
}

void ULOTNHUDWidget::HandleInteractionMessage(const FText& Message)
{
    ShowDialogue(Message);
    RefreshHUD();
}

void ULOTNHUDWidget::HandleInteractClicked()
{
    if (ALOTNCharacter* Player = Cast<ALOTNCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
    {
        Player->InteractPressed();
    }
}

void ULOTNHUDWidget::HandleSaveClicked()
{
    if (UWorld* World = GetWorld())
    {
        if (UGameInstance* Instance = World->GetGameInstance())
        {
            if (ULOTNSaveSubsystem* SaveSystem = Instance->GetSubsystem<ULOTNSaveSubsystem>())
            {
                const bool bSaved = SaveSystem->SaveCurrentGame();
                ShowDialogue(bSaved
                    ? NSLOCTEXT("LOTN", "SaveSuccess", "Progress saved.")
                    : NSLOCTEXT("LOTN", "SaveFailed", "Could not save progress."));
            }
        }
    }
}

void ULOTNHUDWidget::HandleLoadClicked()
{
    if (UWorld* World = GetWorld())
    {
        if (UGameInstance* Instance = World->GetGameInstance())
        {
            if (ULOTNSaveSubsystem* SaveSystem = Instance->GetSubsystem<ULOTNSaveSubsystem>())
            {
                const bool bLoaded = SaveSystem->LoadCurrentGame();
                ShowDialogue(bLoaded
                    ? NSLOCTEXT("LOTN", "LoadSuccess", "Progress loaded.")
                    : NSLOCTEXT("LOTN", "LoadFailed", "No compatible save was found in this location."));
                RefreshHUD();
            }
        }
    }
}
