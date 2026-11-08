#include "LOTNInteractionComponent.h"
#include "LOTNInteractable.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
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

    if (APawn* OwnerPawn = Cast<APawn>(OwnerActor))
    {
        if (AController* OwnerController = OwnerPawn->GetController())
        {
            OwnerController->GetPlayerViewPoint(ViewLocation, ViewRotation);
        }
        else
        {
            OwnerActor->GetActorEyesViewPoint(ViewLocation, ViewRotation);
        }
    }
    else
    {
        OwnerActor->GetActorEyesViewPoint(ViewLocation, ViewRotation);
    }

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
        OnInteractionMessage.Broadcast(NSLOCTEXT("LOTN", "NoInteractable", "Nothing is within reach."));
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
