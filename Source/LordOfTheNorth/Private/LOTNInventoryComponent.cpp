#include "LOTNInventoryComponent.h"

ULOTNInventoryComponent::ULOTNInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

int32 ULOTNInventoryComponent::AddItem(FName ItemId, FText DisplayName, FText Description, ELOTNItemCategory Category, int32 Quantity, int32 MaxStack, ELOTNEquipmentSlot EquipmentSlot)
{
    if (ItemId.IsNone() || Quantity <= 0)
    {
        return Quantity;
    }

    const int32 AcceptedQuantity = FMath::Min(Quantity, 10000);
    const int32 SafeMaxStack = FMath::Max(1, MaxStack);
    int32 Remaining = AcceptedQuantity;

    for (FLOTNInventoryItem& Item : Items)
    {
        if (Item.ItemId == ItemId && Item.Category == Category &&
            Item.EquipmentSlot == EquipmentSlot && Item.Quantity < Item.MaxStack)
        {
            const int32 Space = Item.MaxStack - Item.Quantity;
            const int32 Added = FMath::Min(Space, Remaining);
            Item.Quantity += Added;
            Remaining -= Added;
            if (Remaining == 0)
            {
                OnInventoryChanged.Broadcast();
                return Quantity - AcceptedQuantity;
            }
        }
    }

    while (Remaining > 0)
    {
        FLOTNInventoryItem NewItem;
        NewItem.ItemId = ItemId;
        NewItem.DisplayName = DisplayName;
        NewItem.Description = Description;
        NewItem.Category = Category;
        NewItem.EquipmentSlot = EquipmentSlot;
        NewItem.MaxStack = SafeMaxStack;
        NewItem.Quantity = FMath::Min(SafeMaxStack, Remaining);
        Remaining -= NewItem.Quantity;
        Items.Add(NewItem);
    }

    OnInventoryChanged.Broadcast();
    return Quantity - AcceptedQuantity;
}

bool ULOTNInventoryComponent::RemoveItem(FName ItemId, int32 Quantity)
{
    if (ItemId.IsNone() || Quantity <= 0 || GetItemCount(ItemId) < Quantity)
    {
        return false;
    }

    int32 Remaining = Quantity;
    for (int32 Index = Items.Num() - 1; Index >= 0 && Remaining > 0; --Index)
    {
        FLOTNInventoryItem& Item = Items[Index];
        if (Item.ItemId != ItemId)
        {
            continue;
        }

        const int32 Removed = FMath::Min(Item.Quantity, Remaining);
        Item.Quantity -= Removed;
        Remaining -= Removed;
        if (Item.Quantity <= 0)
        {
            Items.RemoveAt(Index);
        }
    }

    if (GetItemCount(ItemId) == 0)
    {
        EquippedItemIds.Remove(ItemId);
    }

    OnInventoryChanged.Broadcast();
    return true;
}

int32 ULOTNInventoryComponent::GetItemCount(FName ItemId) const
{
    int32 Total = 0;
    for (const FLOTNInventoryItem& Item : Items)
    {
        if (Item.ItemId == ItemId)
        {
            Total = static_cast<int32>(FMath::Min<int64>(MAX_int32, static_cast<int64>(Total) + Item.Quantity));
        }
    }
    return Total;
}

int32 ULOTNInventoryComponent::GetTotalItemCount() const
{
    int64 Total = 0;
    for (const FLOTNInventoryItem& Item : Items)
    {
        Total += FMath::Max(0, Item.Quantity);
    }
    return static_cast<int32>(FMath::Min<int64>(MAX_int32, Total));
}

bool ULOTNInventoryComponent::EquipItem(FName ItemId)
{
    const FLOTNInventoryItem* Item = Items.FindByPredicate(
        [ItemId](const FLOTNInventoryItem& Entry) { return Entry.ItemId == ItemId && Entry.Quantity > 0; });

    if (!Item || Item->EquipmentSlot == ELOTNEquipmentSlot::None ||
        (Item->Category != ELOTNItemCategory::Apparel && Item->Category != ELOTNItemCategory::Trinket))
    {
        return false;
    }

    const ELOTNEquipmentSlot SlotToEquip = Item->EquipmentSlot;
    for (int32 Index = EquippedItemIds.Num() - 1; Index >= 0; --Index)
    {
        const FName EquippedId = EquippedItemIds[Index];
        const FLOTNInventoryItem* EquippedItem = Items.FindByPredicate(
            [EquippedId](const FLOTNInventoryItem& Entry) { return Entry.ItemId == EquippedId; });
        if (EquippedItem && EquippedItem->EquipmentSlot == SlotToEquip)
        {
            EquippedItemIds.RemoveAt(Index);
        }
    }

    EquippedItemIds.AddUnique(ItemId);
    OnInventoryChanged.Broadcast();
    return true;
}

bool ULOTNInventoryComponent::UnequipItem(FName ItemId)
{
    const bool bRemoved = EquippedItemIds.Remove(ItemId) > 0;
    if (bRemoved)
    {
        OnInventoryChanged.Broadcast();
    }
    return bRemoved;
}

void ULOTNInventoryComponent::RestoreInventory(const TArray<FLOTNInventoryItem>& InItems, const TArray<FName>& InEquippedItemIds)
{
    Items = InItems;
    EquippedItemIds = InEquippedItemIds;
    OnInventoryChanged.Broadcast();
}
