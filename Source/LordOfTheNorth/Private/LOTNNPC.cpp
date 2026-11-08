#include "LOTNNPC.h"
#include "LOTNQuestComponent.h"

ALOTNNPC::ALOTNNPC()
{
    PrimaryActorTick.bCanEverTick = false;
    DisplayName = FText::FromString(TEXT("Northern Guard"));
    DialogueText = FText::FromString(TEXT("The road ahead is not safe. Keep your eyes on the tree line."));
    QuestDescription = FText::FromString(TEXT("A supply marker on the northern road needs to be checked. Return here when you have inspected it."));
}

FText ALOTNNPC::GetInteractionText_Implementation(AActor* InteractingActor) const
{
    return FText::Format(NSLOCTEXT("LOTN", "TalkPrompt", "Talk to {0}"), DisplayName);
}

void ALOTNNPC::Interact_Implementation(AActor* InteractingActor)
{
    if (!InteractingActor)
    {
        return;
    }

    if (bOffersQuest)
    {
        if (ULOTNQuestComponent* QuestComponent = InteractingActor->FindComponentByClass<ULOTNQuestComponent>())
        {
            if (QuestComponent->IsQuestReadyToTurnIn(QuestId))
            {
                QuestComponent->CompleteQuest(QuestId);
                OnDialogueRequested.Broadcast(NSLOCTEXT("LOTN", "QuestTurnedIn", "Good work. The North remembers those who keep their word."));
                return;
            }

            if (QuestComponent->IsQuestCompleted(QuestId))
            {
                OnDialogueRequested.Broadcast(NSLOCTEXT("LOTN", "QuestAlreadyDone", "You have already completed this task."));
                return;
            }

            if (QuestComponent->StartQuest(QuestId, QuestTitle, QuestDescription, GoldReward, RenownReward, QuestObjective, RequiredProgress))
            {
                OnDialogueRequested.Broadcast(DialogueText);
                return;
            }

            OnDialogueRequested.Broadcast(NSLOCTEXT("LOTN", "QuestInProgress", "The task is not finished yet. Check the objective on the northern road."));
            return;
        }
    }

    OnDialogueRequested.Broadcast(DialogueText);
}
