#include "LOTNInteractionComponent.h"
#include "LOTNInteractable.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"

ULOTNInteractionComponent::ULOTNInteractionComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

AActor* ULOTNInteractionComponent::FindInteractable() const
{
    AActor* OwnerActor = GetOwner();
    if (!OwnerActor || !GetWorld())
    {
        return nullptr;
    }

    FVector ViewLocation;
    FRotator ViewRotation;
    OwnerActor->GetActorEyesViewPoint(ViewLocation, ViewRotation);

    const FVector End = ViewLocation + ViewRotation.Vector() * InteractionDistance;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(LOTNInteraction), false, OwnerActor);
    FHitResult Hit;

    if (GetWorld()->LineTraceSingleByChannel(Hit, ViewLocation, End, ECC_Visibility, Params))
    {
        AActor* HitActor = Hit.GetActor();
        if (HitActor && HitActor->GetClass()->ImplementsInterface(ULOTNInteractable::StaticClass()))
        {
            return HitActor;
        }
    }

    return nullptr;
}

bool ULOTNInteractionComponent::TryInteract()
{
    AActor* Target = FindInteractable();
    AActor* OwnerActor = GetOwner();
    if (!Target || !OwnerActor)
    {
        OnPromptChanged.Broadcast(FText::GetEmpty());
        return false;
    }

    const FText Message = ILOTNInteractable::Execute_Interact(Target, OwnerActor);
    if (!Message.IsEmpty())
    {
        OnInteractionMessage.Broadcast(Message);
    }
    OnPromptChanged.Broadcast(ILOTNInteractable::Execute_GetInteractionText(Target, OwnerActor));
    return true;
}

FText ULOTNInteractionComponent::GetCurrentPrompt() const
{
    if (AActor* Target = FindInteractable())
    {
        return ILOTNInteractable::Execute_GetInteractionText(Target, GetOwner());
    }
    return FText::GetEmpty();
}
