#include "LOTNNPC.h"
#include "LOTNQuestComponent.h"

ALOTNNPC::ALOTNNPC()
{
    PrimaryActorTick.bCanEverTick = false;
    DisplayName = FText::FromString(TEXT("Northern Guard"));
    DialogueText = FText::FromString(TEXT("The road ahead is not safe. Keep your eyes on the tree line."));
    QuestDescription = FText::FromString(TEXT("Speak with the quartermaster at the Winterfell gate."));
}

FText ALOTNNPC::GetInteractionText_Implementation(AActor* InteractingActor) const
{
    return FText::Format(NSLOCTEXT("LOTN", "TalkPrompt", "Talk to {0}"), DisplayName);
}

void ALOTNNPC::Interact_Implementation(AActor* InteractingActor)
{
    OnDialogueRequested.Broadcast(DialogueText);

    if (!bOffersQuest || !InteractingActor)
    {
        return;
    }

    if (ULOTNQuestComponent* QuestComponent = InteractingActor->FindComponentByClass<ULOTNQuestComponent>())
    {
        QuestComponent->StartQuest(QuestId, QuestTitle, QuestDescription, GoldReward, RenownReward);
    }
}
