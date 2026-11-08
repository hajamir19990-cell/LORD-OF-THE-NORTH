#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LOTNInventoryComponent.generated.h"

UENUM(BlueprintType)
enum class ELOTNItemCategory : uint8
{
    Consumable,
    Material,
    Quest,
    Apparel,
    Trinket
};

USTRUCT(BlueprintType)
struct FLOTNInventoryItem
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
    FName ItemId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory", meta=(MultiLine="true"))
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
    ELOTNItemCategory Category = ELOTNItemCategory::Material;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
    int32 Quantity = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory", meta=(ClampMin="1"))
    int32 MaxStack = 99;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLOTNInventoryChanged);

UCLASS(ClassGroup=(LOTN), meta=(BlueprintSpawnableComponent))
class LORDOFTHENORTH_API ULOTNInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    ULOTNInventoryComponent();

    UPROPERTY(BlueprintAssignable, Category="LOTN|Inventory")
    FLOTNInventoryChanged OnInventoryChanged;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="LOTN|Inventory")
    TArray<FLOTNInventoryItem> Items;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="LOTN|Equipment")
    TArray<FName> EquippedItemIds;

    UFUNCTION(BlueprintCallable, Category="LOTN|Inventory")
    int32 AddItem(FName ItemId, FText DisplayName, FText Description, ELOTNItemCategory Category, int32 Quantity, int32 MaxStack = 99);

    UFUNCTION(BlueprintCallable, Category="LOTN|Inventory")
    bool RemoveItem(FName ItemId, int32 Quantity = 1);

    UFUNCTION(BlueprintPure, Category="LOTN|Inventory")
    int32 GetItemCount(FName ItemId) const;

    UFUNCTION(BlueprintPure, Category="LOTN|Inventory")
    int32 GetTotalItemCount() const;

    UFUNCTION(BlueprintCallable, Category="LOTN|Equipment")
    bool EquipItem(FName ItemId);

    UFUNCTION(BlueprintCallable, Category="LOTN|Equipment")
    bool UnequipItem(FName ItemId);

    UFUNCTION(BlueprintCallable, Category="LOTN|Inventory")
    void RestoreInventory(const TArray<FLOTNInventoryItem>& InItems, const TArray<FName>& InEquippedItemIds);
};
