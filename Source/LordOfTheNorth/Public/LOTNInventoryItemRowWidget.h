#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LOTNInventoryComponent.h"
#include "LOTNInventoryItemRowWidget.generated.h"

class UTextBlock;
class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLOTNInventoryItemEquipRequested, FName, ItemId);

UCLASS()
class LORDOFTHENORTH_API ULOTNInventoryItemRowWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="LOTN|Inventory")
    void SetupRow(const FLOTNInventoryItem& Item, bool bIsEquipped);

    UPROPERTY(BlueprintAssignable, Category="LOTN|Inventory")
    FLOTNInventoryItemEquipRequested OnEquipRequested;

protected:
    virtual void NativeOnInitialized() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

private:
    void BuildNativeLayout();
    void RefreshRow();

    UPROPERTY()
    TObjectPtr<UTextBlock> ItemText;

    UPROPERTY()
    TObjectPtr<UButton> EquipButton;

    UPROPERTY()
    TObjectPtr<UTextBlock> EquipButtonText;

    FName ItemId = NAME_None;
    FText DisplayName;
    int32 Quantity = 0;
    ELOTNItemCategory Category = ELOTNItemCategory::Material;
    ELOTNEquipmentSlot EquipmentSlot = ELOTNEquipmentSlot::None;
    bool bCanEquip = false;
    bool bEquipped = false;

    UFUNCTION()
    void HandleEquipClicked();
};
