#include "LOTNQuestObjective.h"
#include "LOTNQuestComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

ALOTNQuestObjective::ALOTNQuestObjective()
{
    PrimaryActorTick.bCanEverTick = false;

    InteractionBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionBounds"));
    SetRootComponent(InteractionBounds);
    InteractionBounds->SetBoxExtent(FVector(70.f, 70.f, 90.f));
    InteractionBounds->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    InteractionBounds->SetCollisionObjectType(ECC_WorldDynamic);
    InteractionBounds->SetCollisionResponseToAllChannels(ECR_Ignore);
    InteractionBounds->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

    VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
    VisualMesh->SetupAttachment(InteractionBounds);
    VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

FText ALOTNQuestObjective::GetInteractionText_Implementation(AActor* InteractingActor) const
{
    return InteractionLabel;
}

FText ALOTNQuestObjective::Interact_Implementation(AActor* InteractingActor)
{
    if (!InteractingActor)
    {
        return FText::GetEmpty();
    }

    if (ULOTNQuestComponent* Quests = InteractingActor->FindComponentByClass<ULOTNQuestComponent>())
    {
        if (Quests->AdvanceQuestProgress(QuestId, ProgressAmount))
        {
            OnObjectiveActivated.Broadcast();
            return NSLOCTEXT("LOTN", "ObjectiveInspected", "The supply marker has been checked. Return to the guard.");
        }
    }

    return NSLOCTEXT("LOTN", "NoActiveObjective", "There is nothing to inspect here right now.");
}
