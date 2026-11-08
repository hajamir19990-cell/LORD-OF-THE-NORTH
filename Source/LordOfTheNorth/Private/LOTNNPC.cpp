#include "LOTNNPC.h"
#include "LOTNQuestComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

ALOTNNPC::ALOTNNPC()
{
    PrimaryActorTick.bCanEverTick = false;
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    DisplayName = FText::FromString(TEXT("Northern Guard"));
    DialogueText = FText::FromString(TEXT("The road ahead is not safe. Keep your eyes on the tree line."));
    QuestDescription = FText::FromString(TEXT("A supply marker on the northern road needs to be checked. Return here when you have inspected it."));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CapsuleAsset(
        TEXT("/Engine/BasicShapes/Capsule.Capsule"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereAsset(
        TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderAsset(
        TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));

    auto MakePlaceholderPart = [this](FName Name, UStaticMesh* Mesh,
                                      const FVector& Location, const FVector& Scale,
                                      const FRotator& Rotation)
    {
        UStaticMeshComponent* Part = CreateDefaultSubobject<UStaticMeshComponent>(Name);
        Part->SetupAttachment(RootComponent);
        Part->SetRelativeLocation(Location);
        Part->SetRelativeScale3D(Scale);
        Part->SetRelativeRotation(Rotation);
        Part->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        Part->SetCastShadow(true);
        if (Mesh)
        {
            Part->SetStaticMesh(Mesh);
        }
        return Part;
    };

    UStaticMesh* CapsuleMesh = CapsuleAsset.Succeeded() ? CapsuleAsset.Object : nullptr;
    UStaticMesh* SphereMesh = SphereAsset.Succeeded() ? SphereAsset.Object : nullptr;
    UStaticMesh* CylinderMesh = CylinderAsset.Succeeded() ? CylinderAsset.Object : nullptr;

    PlaceholderTorso = MakePlaceholderPart(TEXT("PlaceholderTorso"), CapsuleMesh,
        FVector(0.f, 0.f, -4.f), FVector(0.23f, 0.23f, 0.38f), FRotator::ZeroRotator);
    PlaceholderHead = MakePlaceholderPart(TEXT("PlaceholderHead"), SphereMesh,
        FVector(0.f, 0.f, 52.f), FVector(0.18f, 0.18f, 0.18f), FRotator::ZeroRotator);
    PlaceholderLeftArm = MakePlaceholderPart(TEXT("PlaceholderLeftArm"), CylinderMesh,
        FVector(0.f, -25.f, -5.f), FVector(0.085f, 0.085f, 0.28f), FRotator(0.f, 0.f, 8.f));
    PlaceholderRightArm = MakePlaceholderPart(TEXT("PlaceholderRightArm"), CylinderMesh,
        FVector(0.f, 25.f, -5.f), FVector(0.085f, 0.085f, 0.28f), FRotator(0.f, 0.f, -8.f));
    PlaceholderLeftLeg = MakePlaceholderPart(TEXT("PlaceholderLeftLeg"), CylinderMesh,
        FVector(0.f, -11.f, -62.f), FVector(0.095f, 0.095f, 0.34f), FRotator::ZeroRotator);
    PlaceholderRightLeg = MakePlaceholderPart(TEXT("PlaceholderRightLeg"), CylinderMesh,
        FVector(0.f, 11.f, -62.f), FVector(0.095f, 0.095f, 0.34f), FRotator::ZeroRotator);
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
