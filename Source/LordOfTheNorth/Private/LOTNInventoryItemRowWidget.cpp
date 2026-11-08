#include "LOTNInventoryItemRowWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Blueprint/WidgetTree.h"
#include "Styling/CoreStyle.h"

void ULOTNInventoryItemRowWidget::BuildNativeLayout()
{
    if (!WidgetTree || WidgetTree->RootWidget)
    {
        return;
    }

    UHorizontalBox* Root = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("InventoryItemRow"));
    WidgetTree->RootWidget = Root;

    ItemText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ItemText"));
    ItemText->SetColorAndOpacity(FSlateColor(FLinearColor(0.88f, 0.91f, 0.92f, 1.f)));
    ItemText->SetFont(FCoreStyle::GetDefaultFontStyle("Regular", 12));
    ItemText->SetAutoWrapText(true);
    Root->AddChildToHorizontalBox(ItemText);

    EquipButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("EquipButton"));
    EquipButton->SetBackgroundColor(FLinearColor(0.12f, 0.16f, 0.19f, 0.96f));
    EquipButtonText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("EquipButtonText"));
    EquipButtonText->SetColorAndOpacity(FSlateColor(FLinearColor(0.88f, 0.91f, 0.92f, 1.f)));
    EquipButtonText->SetFont(FCoreStyle::GetDefaultFontStyle("Bold", 10));
    EquipButtonText->SetJustification(ETextJustify::Center);
    EquipButton->AddChild(EquipButtonText);
    Root->AddChildToHorizontalBox(EquipButton);
}

void ULOTNInventoryItemRowWidget::NativeOnInitialized()
{
    BuildNativeLayout();
    Super::NativeOnInitialized();
}

void ULOTNInventoryItemRowWidget::NativeConstruct()
{
    Super::NativeConstruct();
    if (EquipButton)
    {
        EquipButton->OnClicked.AddDynamic(this, &ULOTNInventoryItemRowWidget::HandleEquipClicked);
    }
    RefreshRow();
}

void ULOTNInventoryItemRowWidget::NativeDestruct()
{
    if (EquipButton)
    {
        EquipButton->OnClicked.RemoveDynamic(this, &ULOTNInventoryItemRowWidget::HandleEquipClicked);
    }
    Super::NativeDestruct();
}

void ULOTNInventoryItemRowWidget::SetupRow(const FLOTNInventoryItem& Item, bool bIsEquipped)
{
    ItemId = Item.ItemId;
    DisplayName = Item.DisplayName;
    Quantity = Item.Quantity;
    Category = Item.Category;
    EquipmentSlot = Item.EquipmentSlot;
    bCanEquip = EquipmentSlot != ELOTNEquipmentSlot::None &&
        (Category == ELOTNItemCategory::Apparel || Category == ELOTNItemCategory::Trinket);
    bEquipped = bIsEquipped;
    RefreshRow();
}

void ULOTNInventoryItemRowWidget::RefreshRow()
{
    if (ItemText)
    {
        ItemText->SetText(FText::Format(
            NSLOCTEXT("LOTN", "InventoryRowFormat", "{0}  x{1}"),
            DisplayName,
            FText::AsNumber(Quantity)));
    }

    if (EquipButton)
    {
        EquipButton->SetVisibility(bCanEquip ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
        EquipButton->SetIsEnabled(bCanEquip);
    }

    if (EquipButtonText)
    {
        EquipButtonText->SetText(bEquipped
            ? NSLOCTEXT("LOTN", "UnequipItem", "UNEQUIP")
            : NSLOCTEXT("LOTN", "EquipItem", "EQUIP"));
    }
}

void ULOTNInventoryItemRowWidget::HandleEquipClicked()
{
    if (bCanEquip && !ItemId.IsNone())
    {
        OnEquipRequested.Broadcast(ItemId);
    }
}
