#include "LOTNNPC.h"
#include "LOTNQuestComponent.h"
#include "Components/CapsuleComponent.h"

ALOTNNPC::ALOTNNPC()
{
    PrimaryActorTick.bCanEverTick = false;
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    DisplayName = FText::FromString(TEXT("Northern Guard"));
    DialogueText = FText::FromString(TEXT("The road ahead is not safe. Keep your eyes on the tree line."));
    QuestDescription = FText::FromString(TEXT("A supply marker on the northern road needs to be checked. Return here when you have inspected it."));
}

FText ALOTNNPC::GetInteractionText_Implementation(AActor* InteractingActor) const
{
    return FText::Format(NSLOCTEXT("LOTN", "TalkPrompt", "Talk to {0}"), DisplayName);
}

FText ALOTNNPC::Interact_Implementation(AActor* InteractingActor)
{
    FText Message = DialogueText;
    if (InteractingActor && bOffersQuest)
    {
        if (ULOTNQuestComponent* QuestComponent = InteractingActor->FindComponentByClass<ULOTNQuestComponent>())
        {
            if (QuestComponent->IsQuestReadyToTurnIn(QuestId))
            {
                QuestComponent->CompleteQuest(QuestId);
                Message = NSLOCTEXT("LOTN", "QuestTurnedIn", "Good work. The North remembers those who keep their word.");
            }
            else if (QuestComponent->IsQuestCompleted(QuestId))
            {
                Message = NSLOCTEXT("LOTN", "QuestAlreadyDone", "You have already completed this task.");
            }
            else if (!QuestComponent->StartQuest(QuestId, QuestTitle, QuestDescription, GoldReward, RenownReward, QuestObjective, RequiredProgress))
            {
                Message = NSLOCTEXT("LOTN", "QuestInProgress", "The task is not finished yet. Check the objective on the northern road.");
            }
        }
    }

    OnDialogueRequested.Broadcast(Message);
    return Message;
}
