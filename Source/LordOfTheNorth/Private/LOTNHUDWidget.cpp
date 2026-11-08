#include "LOTNHUDWidget.h"
#include "LOTNCharacter.h"
#include "LOTNPlayerStatsComponent.h"
#include "LOTNQuestComponent.h"
#include "LOTNInventoryComponent.h"
#include "LOTNInventoryItemRowWidget.h"
#include "LOTNInteractionComponent.h"
#include "LOTNSaveSubsystem.h"
#include "LOTNWorldMapData.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/VerticalBox.h"
#include "Components/HorizontalBox.h"
#include "Components/Border.h"
#include "Components/SizeBox.h"
#include "Components/ScrollBox.h"
#include "Blueprint/WidgetTree.h"
#include "Styling/CoreStyle.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"

void ULOTNHUDWidget::BuildNativeLayout()
{
    if (!WidgetTree || WidgetTree->RootWidget)
    {
        return;
    }

    UCanvasPanel* Canvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("NativeRootCanvas"));
    WidgetTree->RootWidget = Canvas;

    auto AddPanel = [this, Canvas](FName Name, const FAnchors& Anchors, const FVector2D& Alignment,
                                   const FVector2D& Position, const FVector2D& Size) -> UVerticalBox*
    {
        UBorder* Border = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), Name);
        Border->SetBrushColor(FLinearColor(0.025f, 0.04f, 0.06f, 0.88f));
        Border->SetPadding(FMargin(10.f, 8.f, 10.f, 8.f));
        UVerticalBox* Box = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
        Border->SetContent(Box);
        if (UCanvasPanelSlot* Slot = Canvas->AddChildToCanvas(Border))
        {
            Slot->SetAnchors(Anchors);
            Slot->SetAlignment(Alignment);
            Slot->SetPosition(Position);
            Slot->SetSize(Size);
        }
        return Box;
    };

    auto AddText = [this](UVerticalBox* Parent, const TCHAR* Text, int32 FontSize, const FLinearColor& Color) -> UTextBlock*
    {
        UTextBlock* Label = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
        Label->SetText(FText::FromString(Text));
        Label->SetColorAndOpacity(FSlateColor(Color));
        Label->SetFont(FCoreStyle::GetDefaultFontStyle("Bold", FontSize));
        Parent->AddChildToVerticalBox(Label);
        return Label;
    };

    auto AddBar = [this](UVerticalBox* Parent, const FLinearColor& FillColor) -> UProgressBar*
    {
        UProgressBar* Bar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass());
        Bar->SetPercent(1.f);
        Bar->SetFillColorAndOpacity(FillColor);
        USizeBox* Wrapper = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
        Wrapper->SetWidthOverride(190.f);
        Wrapper->SetHeightOverride(13.f);
        Wrapper->AddChild(Bar);
        Parent->AddChildToVerticalBox(Wrapper);
        return Bar;
    };

    const FLinearColor TextColor(0.88f, 0.91f, 0.92f, 1.f);
    UVerticalBox* StatsPanel = AddPanel(TEXT("StatsPanel"), FAnchors(0.f, 0.f, 0.f, 0.f),
        FVector2D(0.f, 0.f), FVector2D(14.f, 14.f), FVector2D(218.f, 174.f));
    AddText(StatsPanel, TEXT("LORD OF THE NORTH"), 16, FLinearColor(0.78f, 0.82f, 0.85f, 1.f));
    AddText(StatsPanel, TEXT("HEALTH"), 10, TextColor);
    HealthBar = AddBar(StatsPanel, FLinearColor(0.55f, 0.12f, 0.12f, 1.f));
    AddText(StatsPanel, TEXT("STAMINA"), 10, TextColor);
    StaminaBar = AddBar(StatsPanel, FLinearColor(0.72f, 0.62f, 0.30f, 1.f));
    GoldText = AddText(StatsPanel, TEXT("Gold: 0"), 12, TextColor);
    RenownText = AddText(StatsPanel, TEXT("Renown: 0"), 12, TextColor);
    InventoryCountText = AddText(StatsPanel, TEXT("Inventory: 0"), 12, TextColor);

    UVerticalBox* QuestPanel = AddPanel(TEXT("QuestPanel"), FAnchors(0.f, 0.f, 0.f, 0.f),
        FVector2D(0.f, 0.f), FVector2D(14.f, 196.f), FVector2D(292.f, 118.f));
    AddText(QuestPanel, TEXT("CURRENT TASK"), 12, FLinearColor(0.82f, 0.75f, 0.55f, 1.f));
    QuestTitleText = AddText(QuestPanel, TEXT("No active quest"), 14, TextColor);
    QuestObjectiveText = AddText(QuestPanel, TEXT(""), 11, TextColor);
    QuestObjectiveText->SetAutoWrapText(true);
    QuestProgressText = AddText(QuestPanel, TEXT(""), 12, TextColor);

    InteractionPromptPanel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("InteractionPromptPanel"));
    InteractionPromptPanel->SetBrushColor(FLinearColor(0.025f, 0.04f, 0.06f, 0.88f));
    InteractionPromptPanel->SetPadding(FMargin(8.f, 4.f, 8.f, 4.f));
    InteractionPromptText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("InteractionPromptText"));
    InteractionPromptText->SetColorAndOpacity(FSlateColor(TextColor));
    InteractionPromptText->SetFont(FCoreStyle::GetDefaultFontStyle("Bold", 13));
    InteractionPromptText->SetAutoWrapText(true);
    InteractionPromptPanel->SetContent(InteractionPromptText);
    if (UCanvasPanelSlot* Slot = Canvas->AddChildToCanvas(InteractionPromptPanel))
    {
        Slot->SetAnchors(FAnchors(0.5f, 1.f, 0.5f, 1.f));
        Slot->SetAlignment(FVector2D(0.5f, 1.f));
        Slot->SetPosition(FVector2D(0.f, -250.f));
        Slot->SetSize(FVector2D(280.f, 34.f));
    }
    InteractionPromptText->SetVisibility(ESlateVisibility::Collapsed);

    DialoguePanel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("DialoguePanel"));
    DialoguePanel->SetBrushColor(FLinearColor(0.025f, 0.04f, 0.06f, 0.92f));
    DialoguePanel->SetPadding(FMargin(10.f, 8.f, 10.f, 8.f));
    DialogueText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("DialogueText"));
    DialogueText->SetColorAndOpacity(FSlateColor(TextColor));
    DialogueText->SetFont(FCoreStyle::GetDefaultFontStyle("Regular", 13));
    DialogueText->SetAutoWrapText(true);
    DialoguePanel->SetContent(DialogueText);
    if (UCanvasPanelSlot* Slot = Canvas->AddChildToCanvas(DialoguePanel))
    {
        Slot->SetAnchors(FAnchors(0.5f, 1.f, 0.5f, 1.f));
        Slot->SetAlignment(FVector2D(0.5f, 1.f));
        Slot->SetPosition(FVector2D(0.f, -180.f));
        Slot->SetSize(FVector2D(320.f, 64.f));
    }
    DialoguePanel->SetVisibility(ESlateVisibility::Collapsed);

    auto AddButton = [this, Canvas, &TextColor](FName Name, const TCHAR* LabelText,
        const FAnchors& Anchors, const FVector2D& Alignment,
        const FVector2D& Position, const FVector2D& Size) -> UButton*
    {
        UButton* Button = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), Name);
        Button->SetBackgroundColor(FLinearColor(0.12f, 0.16f, 0.19f, 0.96f));
        UTextBlock* Label = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
        Label->SetText(FText::FromString(LabelText));
        Label->SetColorAndOpacity(FSlateColor(TextColor));
        Label->SetFont(FCoreStyle::GetDefaultFontStyle("Bold", 12));
        Label->SetJustification(ETextJustify::Center);
        Button->AddChild(Label);
        if (UCanvasPanelSlot* Slot = Canvas->AddChildToCanvas(Button))
        {
            Slot->SetAnchors(Anchors);
            Slot->SetAlignment(Alignment);
            Slot->SetPosition(Position);
            Slot->SetSize(Size);
        }
        return Button;
    };

    InventoryButton = AddButton(TEXT("InventoryButton"), TEXT("BAG"),
        FAnchors(1.f, 0.f, 1.f, 0.f), FVector2D(1.f, 0.f), FVector2D(-14.f, 14.f), FVector2D(70.f, 40.f));

    InventoryPanel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("InventoryPanel"));
    InventoryPanel->SetBrushColor(FLinearColor(0.018f, 0.028f, 0.04f, 0.97f));
    InventoryPanel->SetPadding(FMargin(12.f, 10.f, 12.f, 10.f));
    UVerticalBox* InventoryRoot = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
    UHorizontalBox* InventoryHeader = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
    UTextBlock* InventoryTitle = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
    InventoryTitle->SetText(NSLOCTEXT("LOTN", "InventoryTitle", "INVENTORY"));
    InventoryTitle->SetColorAndOpacity(FSlateColor(FLinearColor(0.82f, 0.75f, 0.55f, 1.f)));
    InventoryTitle->SetFont(FCoreStyle::GetDefaultFontStyle("Bold", 16));
    InventoryHeader->AddChildToHorizontalBox(InventoryTitle);

    InventoryCloseButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("InventoryCloseButton"));
    InventoryCloseButton->SetBackgroundColor(FLinearColor(0.18f, 0.12f, 0.12f, 1.f));
    UTextBlock* CloseLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
    CloseLabel->SetText(NSLOCTEXT("LOTN", "CloseInventory", "CLOSE"));
    CloseLabel->SetColorAndOpacity(FSlateColor(TextColor));
    CloseLabel->SetFont(FCoreStyle::GetDefaultFontStyle("Bold", 11));
    InventoryCloseButton->AddChild(CloseLabel);
    InventoryHeader->AddChildToHorizontalBox(InventoryCloseButton);
    InventoryRoot->AddChildToVerticalBox(InventoryHeader);

    InventoryList = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass(), TEXT("InventoryList"));
    InventoryRoot->AddChildToVerticalBox(InventoryList);
    InventoryPanel->SetContent(InventoryRoot);
    if (UCanvasPanelSlot* Slot = Canvas->AddChildToCanvas(InventoryPanel))
    {
        Slot->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
        Slot->SetAlignment(FVector2D(0.5f, 0.5f));
        Slot->SetPosition(FVector2D(0.f, 0.f));
        Slot->SetSize(FVector2D(292.f, 360.f));
    }
    InventoryPanel->SetVisibility(ESlateVisibility::Collapsed);

    InteractButton = AddButton(TEXT("InteractButton"), TEXT("INTERACT"),
        FAnchors(0.5f, 1.f, 0.5f, 1.f), FVector2D(0.5f, 1.f), FVector2D(-38.f, -112.f), FVector2D(76.f, 64.f));
    SprintButton = AddButton(TEXT("SprintButton"), TEXT("SPRINT"),
        FAnchors(1.f, 1.f, 1.f, 1.f), FVector2D(1.f, 1.f), FVector2D(-14.f, -112.f), FVector2D(78.f, 64.f));
    // Keep the lower-left quadrant clear for Unreal's landscape virtual movement joystick.
    // Save/load controls live beneath the inventory button in the upper-right corner.
    SaveButton = AddButton(TEXT("SaveButton"), TEXT("SAVE"),
        FAnchors(1.f, 0.f, 1.f, 0.f), FVector2D(1.f, 0.f), FVector2D(-160.f, 62.f), FVector2D(66.f, 38.f));
    LoadButton = AddButton(TEXT("LoadButton"), TEXT("LOAD"),
        FAnchors(1.f, 0.f, 1.f, 0.f), FVector2D(1.f, 0.f), FVector2D(-86.f, 62.f), FVector2D(66.f, 38.f));
    MapButton = AddButton(TEXT("MapButton"), TEXT("MAP"),
        FAnchors(1.f, 0.f, 1.f, 0.f), FVector2D(1.f, 0.f), FVector2D(-92.f, 14.f), FVector2D(70.f, 40.f));

    BuildWorldMapPanel(Canvas);
}

void ULOTNHUDWidget::BuildWorldMapPanel(UCanvasPanel* ParentCanvas)
{
    if (!WidgetTree || !ParentCanvas)
    {
        return;
    }

    WorldMapData = NewObject<ULOTNWorldMapData>(this);
    if (!WorldMapData)
    {
        return;
    }

    WorldMapPanel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("WorldMapPanel"));
    WorldMapPanel->SetBrushColor(FLinearColor(0.018f, 0.028f, 0.04f, 0.98f));
    WorldMapPanel->SetPadding(FMargin(14.f, 12.f, 14.f, 12.f));

    UVerticalBox* MapRoot = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
    UHorizontalBox* MapHeader = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());

    UTextBlock* MapTitle = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
    MapTitle->SetText(NSLOCTEXT("LOTN", "NorthernMapTitle", "THE NORTH  /  REGIONAL MAP"));
    MapTitle->SetColorAndOpacity(FSlateColor(FLinearColor(0.82f, 0.75f, 0.55f, 1.f)));
    MapTitle->SetFont(FCoreStyle::GetDefaultFontStyle("Bold", 16));
    MapHeader->AddChildToHorizontalBox(MapTitle);

    WorldMapCloseButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("WorldMapCloseButton"));
    WorldMapCloseButton->SetBackgroundColor(FLinearColor(0.18f, 0.12f, 0.12f, 1.f));
    UTextBlock* CloseLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
    CloseLabel->SetText(NSLOCTEXT("LOTN", "CloseWorldMap", "CLOSE"));
    CloseLabel->SetColorAndOpacity(FSlateColor(FLinearColor(0.88f, 0.91f, 0.92f, 1.f)));
    CloseLabel->SetFont(FCoreStyle::GetDefaultFontStyle("Bold", 11));
    WorldMapCloseButton->AddChild(CloseLabel);
    MapHeader->AddChildToHorizontalBox(WorldMapCloseButton);
    MapRoot->AddChildToVerticalBox(MapHeader);

    USizeBox* MapArea = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
    MapArea->SetWidthOverride(600.f);
    MapArea->SetHeightOverride(220.f);

    UBorder* MapFrame = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
    MapFrame->SetBrushColor(FLinearColor(0.075f, 0.095f, 0.105f, 1.f));
    MapFrame->SetPadding(FMargin(4.f));
    MapFrame->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    WorldMapCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("WorldMapCanvas"));
    MapFrame->SetContent(WorldMapCanvas);
    MapArea->AddChild(MapFrame);
    MapRoot->AddChildToVerticalBox(MapArea);

    WorldMapDetailsText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("WorldMapDetailsText"));
    WorldMapDetailsText->SetColorAndOpacity(FSlateColor(FLinearColor(0.86f, 0.89f, 0.90f, 1.f)));
    WorldMapDetailsText->SetFont(FCoreStyle::GetDefaultFontStyle("Regular", 12));
    WorldMapDetailsText->SetAutoWrapText(true);
    MapRoot->AddChildToVerticalBox(WorldMapDetailsText);

    WorldMapPanel->SetContent(MapRoot);
    if (UCanvasPanelSlot* Slot = ParentCanvas->AddChildToCanvas(WorldMapPanel))
    {
        Slot->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
        Slot->SetAlignment(FVector2D(0.5f, 0.5f));
        Slot->SetPosition(FVector2D(0.f, 0.f));
        Slot->SetSize(FVector2D(640.f, 350.f));
    }
    WorldMapPanel->SetVisibility(ESlateVisibility::Collapsed);

    // Draw each undirected road once, then place all nine regional nodes over the route graph.
    TSet<FString> DrawnRoutes;
    for (const FLOTNWorldLocation& Location : WorldMapData->Locations)
    {
        for (const FName ConnectedId : Location.ConnectedLocationIds)
        {
            FString RouteA = Location.LocationId.ToString();
            FString RouteB = ConnectedId.ToString();
            if (RouteA.Compare(RouteB, ESearchCase::CaseSensitive) > 0)
            {
                Swap(RouteA, RouteB);
            }
            const FString RouteKey = RouteA + TEXT("|") + RouteB;
            if (DrawnRoutes.Contains(RouteKey))
            {
                continue;
            }
            DrawnRoutes.Add(RouteKey);

            const FLOTNWorldLocation* Connected = WorldMapData->Locations.FindByPredicate(
                [ConnectedId](const FLOTNWorldLocation& Candidate)
                {
                    return Candidate.LocationId == ConnectedId;
                });
            if (!Connected)
            {
                continue;
            }

            const FVector2D A(10.f + Location.MapPosition.X * 560.f,
                              8.f + Location.MapPosition.Y * 195.f);
            const FVector2D B(10.f + Connected->MapPosition.X * 560.f,
                              8.f + Connected->MapPosition.Y * 195.f);
            const FVector2D Delta = B - A;
            UBorder* RouteLine = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
            RouteLine->SetBrushColor(FLinearColor(0.48f, 0.50f, 0.47f, 0.62f));
            RouteLine->SetVisibility(ESlateVisibility::HitTestInvisible);
            if (UCanvasPanelSlot* LineSlot = WorldMapCanvas->AddChildToCanvas(RouteLine))
            {
                LineSlot->SetAnchors(FAnchors(0.f, 0.f, 0.f, 0.f));
                LineSlot->SetAlignment(FVector2D(0.5f, 0.5f));
                LineSlot->SetPosition((A + B) * 0.5f);
                LineSlot->SetSize(FVector2D(Delta.Size(), 3.f));
            }
            RouteLine->SetRenderTransformAngle(FMath::RadiansToDegrees(FMath::Atan2(Delta.Y, Delta.X)));
        }
    }

    for (const FLOTNWorldLocation& Location : WorldMapData->Locations)
    {
        UButton* Node = WidgetTree->ConstructWidget<UButton>(
            UButton::StaticClass(),
            FName(*FString::Printf(TEXT("WorldMapNode_%s"), *Location.LocationId.ToString())));
        Node->SetBackgroundColor(FLinearColor(0.16f, 0.19f, 0.19f, 1.f));

        UTextBlock* NodeLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
        NodeLabel->SetText(Location.DisplayName);
        NodeLabel->SetColorAndOpacity(FSlateColor(FLinearColor(0.92f, 0.93f, 0.91f, 1.f)));
        NodeLabel->SetFont(FCoreStyle::GetDefaultFontStyle("Bold", 10));
        NodeLabel->SetJustification(ETextJustify::Center);
        Node->AddChild(NodeLabel);

        if (UCanvasPanelSlot* NodeSlot = WorldMapCanvas->AddChildToCanvas(Node))
        {
            NodeSlot->SetAnchors(FAnchors(0.f, 0.f, 0.f, 0.f));
            NodeSlot->SetAlignment(FVector2D(0.5f, 0.5f));
            NodeSlot->SetPosition(FVector2D(10.f + Location.MapPosition.X * 560.f,
                                            8.f + Location.MapPosition.Y * 195.f));
            NodeSlot->SetSize(FVector2D(96.f, 30.f));
        }

        const FName LocationId = Location.LocationId;
        Node->OnClicked.AddLambda([this, LocationId]()
        {
            SelectWorldMapLocation(LocationId);
        });
        WorldMapNodeButtons.Add(Location.LocationId, Node);
    }

    RefreshWorldMapPanel();
    SelectWorldMapLocation(TEXT("Winterfell"));
}

void ULOTNHUDWidget::RefreshWorldMapPanel()
{
    if (!WorldMapData)
    {
        return;
    }

    const ALOTNCharacter* Player = Cast<ALOTNCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    const int32 CurrentRenown = Player && Player->PlayerStats ? Player->PlayerStats->Renown : 0;

    for (const FLOTNWorldLocation& Location : WorldMapData->Locations)
    {
        if (TObjectPtr<UButton>* Node = WorldMapNodeButtons.Find(Location.LocationId))
        {
            const bool bUnlocked = WorldMapData->IsLocationUnlocked(Location.LocationId, CurrentRenown);
            (*Node)->SetBackgroundColor(bUnlocked
                ? FLinearColor(0.22f, 0.28f, 0.24f, 1.f)
                : FLinearColor(0.14f, 0.16f, 0.18f, 1.f));
        }
    }

    if (!SelectedWorldMapLocation.IsNone())
    {
        SelectWorldMapLocation(SelectedWorldMapLocation);
    }
}

void ULOTNHUDWidget::SelectWorldMapLocation(FName LocationId)
{
    if (!WorldMapData || !WorldMapDetailsText)
    {
        return;
    }

    FLOTNWorldLocation Location;
    if (!WorldMapData->GetLocationById(LocationId, Location))
    {
        return;
    }

    SelectedWorldMapLocation = LocationId;
    const ALOTNCharacter* Player = Cast<ALOTNCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    const int32 CurrentRenown = Player && Player->PlayerStats ? Player->PlayerStats->Renown : 0;
    const bool bUnlocked = WorldMapData->IsLocationUnlocked(LocationId, CurrentRenown);

    FString Status;
    if (LocationId == FName(TEXT("Winterfell")))
    {
        Status = TEXT("CURRENT REGION");
    }
    else if (bUnlocked)
    {
        Status = TEXT("UNLOCKED");
    }
    else
    {
        Status = FString::Printf(TEXT("LOCKED  /  %d RENOWN REQUIRED"), Location.RequiredRenown);
    }

    FString TravelStatus;
    if (LocationId == FName(TEXT("Winterfell")))
    {
        TravelStatus = TEXT("You are already in this region.");
    }
    else if (WorldMapData->IsLocationReadyToTravel(LocationId, CurrentRenown))
    {
        TravelStatus = TEXT("Travel destination available.");
    }
    else
    {
        TravelStatus = bUnlocked
            ? TEXT("Region map is not built yet; travel is unavailable.")
            : TEXT("Earn more renown to unlock this region.");
    }

    for (const FLOTNWorldLocation& Entry : WorldMapData->Locations)
    {
        if (TObjectPtr<UButton>* Node = WorldMapNodeButtons.Find(Entry.LocationId))
        {
            const bool bEntryUnlocked = WorldMapData->IsLocationUnlocked(Entry.LocationId, CurrentRenown);
            (*Node)->SetBackgroundColor(Entry.LocationId == LocationId
                ? FLinearColor(0.48f, 0.36f, 0.17f, 1.f)
                : (bEntryUnlocked
                    ? FLinearColor(0.22f, 0.28f, 0.24f, 1.f)
                    : FLinearColor(0.14f, 0.16f, 0.18f, 1.f)));
        }
    }

    const FString RenownInfo = FString::Printf(
        TEXT("Renown: %d current  /  %d required"),
        CurrentRenown,
        Location.RequiredRenown);

    FString Routes;
    for (const FName ConnectedId : Location.ConnectedLocationIds)
    {
        FLOTNWorldLocation Connected;
        if (WorldMapData->GetLocationById(ConnectedId, Connected))
        {
            if (!Routes.IsEmpty())
            {
                Routes += TEXT("  •  ");
            }
            Routes += Connected.DisplayName.ToString();
        }
    }

    WorldMapDetailsText->SetText(FText::FromString(FString::Printf(
        TEXT("%s  |  %s  |  %s\n%s\n%s\n%s\nRoutes: %s"),
        *Location.DisplayName.ToString(),
        *Location.RegionType.ToString(),
        *Status,
        *Location.Description.ToString(),
        *TravelStatus,
        *RenownInfo,
        *Routes)));
}

void ULOTNHUDWidget::ToggleWorldMapPanel(bool bShow)
{
    if (WorldMapPanel)
    {
        WorldMapPanel->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
        if (bShow)
        {
            ToggleInventoryPanel(false);
            RefreshWorldMapPanel();
        }
    }
}

void ULOTNHUDWidget::HandleWorldMapToggleClicked()
{
    if (WorldMapPanel)
    {
        ToggleWorldMapPanel(WorldMapPanel->GetVisibility() != ESlateVisibility::Visible);
    }
}

void ULOTNHUDWidget::HandleWorldMapCloseClicked()
{
    ToggleWorldMapPanel(false);
}

void ULOTNHUDWidget::NativeOnInitialized()
{
    BuildNativeLayout();
    Super::NativeOnInitialized();
}

void ULOTNHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();
    BindPlayerEvents(true);
    CachedInteractionPrompt = FText::GetEmpty();
    PromptRefreshAccumulator = 0.f;

    if (DialoguePanel)
    {
        DialoguePanel->SetVisibility(ESlateVisibility::Collapsed);
    }
    if (InteractionPromptPanel)
    {
        InteractionPromptPanel->SetVisibility(ESlateVisibility::Collapsed);
    }

    if (InteractButton)
    {
        InteractButton->OnClicked.AddDynamic(this, &ULOTNHUDWidget::HandleInteractClicked);
    }
    if (InventoryButton)
    {
        InventoryButton->OnClicked.AddDynamic(this, &ULOTNHUDWidget::HandleInventoryToggleClicked);
    }
    if (MapButton)
    {
        MapButton->OnClicked.AddDynamic(this, &ULOTNHUDWidget::HandleWorldMapToggleClicked);
    }
    if (WorldMapCloseButton)
    {
        WorldMapCloseButton->OnClicked.AddDynamic(this, &ULOTNHUDWidget::HandleWorldMapCloseClicked);
    }
    if (InventoryCloseButton)
    {
        InventoryCloseButton->OnClicked.AddDynamic(this, &ULOTNHUDWidget::HandleInventoryCloseClicked);
    }
    if (InventoryPanel)
    {
        InventoryPanel->SetVisibility(ESlateVisibility::Collapsed);
    }
    RefreshInventoryPanel();

    if (SprintButton)
    {
        SprintButton->OnPressed.AddDynamic(this, &ULOTNHUDWidget::HandleSprintPressed);
        SprintButton->OnReleased.AddDynamic(this, &ULOTNHUDWidget::HandleSprintReleased);
    }
    if (SaveButton)
    {
        SaveButton->OnClicked.AddDynamic(this, &ULOTNHUDWidget::HandleSaveClicked);
    }
    if (LoadButton)
    {
        LoadButton->OnClicked.AddDynamic(this, &ULOTNHUDWidget::HandleLoadClicked);
    }

    RefreshHUD();
    RefreshWorldMapPanel();
}

void ULOTNHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    PromptRefreshAccumulator += InDeltaTime;
    if (PromptRefreshAccumulator < PromptRefreshInterval)
    {
        return;
    }
    PromptRefreshAccumulator = 0.f;

    ALOTNCharacter* CurrentPlayer = Cast<ALOTNCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    if (CurrentPlayer != BoundPlayer.Get())
    {
        BindPlayerEvents(false);
        if (CurrentPlayer)
        {
            BindPlayerEvents(true);
        }
        RefreshHUD();
        RefreshWorldMapPanel();
    }

    if (!InteractionPromptText)
    {
        return;
    }

    const FText Prompt = CurrentPlayer && CurrentPlayer->InteractionComponent
        ? CurrentPlayer->InteractionComponent->GetCurrentPrompt()
        : FText::GetEmpty();

    if (!Prompt.EqualTo(CachedInteractionPrompt))
    {
        CachedInteractionPrompt = Prompt;
        InteractionPromptText->SetText(Prompt);
        if (InteractionPromptPanel)
        {
            InteractionPromptPanel->SetVisibility(Prompt.IsEmpty() ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
        }
        else
        {
            InteractionPromptText->SetVisibility(Prompt.IsEmpty() ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
        }
    }
}

void ULOTNHUDWidget::NativeDestruct()
{
    HandleSprintReleased();
    BindPlayerEvents(false);

    if (InteractButton)
    {
        InteractButton->OnClicked.RemoveDynamic(this, &ULOTNHUDWidget::HandleInteractClicked);
    }
    if (InventoryButton)
    {
        InventoryButton->OnClicked.RemoveDynamic(this, &ULOTNHUDWidget::HandleInventoryToggleClicked);
    }
    if (MapButton)
    {
        MapButton->OnClicked.RemoveDynamic(this, &ULOTNHUDWidget::HandleWorldMapToggleClicked);
    }
    if (WorldMapCloseButton)
    {
        WorldMapCloseButton->OnClicked.RemoveDynamic(this, &ULOTNHUDWidget::HandleWorldMapCloseClicked);
    }
    if (InventoryCloseButton)
    {
        InventoryCloseButton->OnClicked.RemoveDynamic(this, &ULOTNHUDWidget::HandleInventoryCloseClicked);
    }
    if (SprintButton)
    {
        SprintButton->OnPressed.RemoveDynamic(this, &ULOTNHUDWidget::HandleSprintPressed);
        SprintButton->OnReleased.RemoveDynamic(this, &ULOTNHUDWidget::HandleSprintReleased);
    }
    if (SaveButton)
    {
        SaveButton->OnClicked.RemoveDynamic(this, &ULOTNHUDWidget::HandleSaveClicked);
    }
    if (LoadButton)
    {
        LoadButton->OnClicked.RemoveDynamic(this, &ULOTNHUDWidget::HandleLoadClicked);
    }

    Super::NativeDestruct();
}

void ULOTNHUDWidget::BindPlayerEvents(bool bBind)
{
    ALOTNCharacter* Player = bBind
        ? Cast<ALOTNCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0))
        : BoundPlayer.Get();
    if (!Player)
    {
        if (!bBind)
        {
            BoundPlayer.Reset();
        }
        return;
    }

    if (Player->PlayerStats)
    {
        if (bBind)
        {
            Player->PlayerStats->OnStatsChanged.AddDynamic(this, &ULOTNHUDWidget::HandleStatsChanged);
        }
        else
        {
            Player->PlayerStats->OnStatsChanged.RemoveDynamic(this, &ULOTNHUDWidget::HandleStatsChanged);
        }
    }

    if (Player->QuestComponent)
    {
        if (bBind)
        {
            Player->QuestComponent->OnQuestStateChanged.AddDynamic(this, &ULOTNHUDWidget::HandleQuestChanged);
        }
        else
        {
            Player->QuestComponent->OnQuestStateChanged.RemoveDynamic(this, &ULOTNHUDWidget::HandleQuestChanged);
        }
    }

    if (Player->InventoryComponent)
    {
        if (bBind)
        {
            Player->InventoryComponent->OnInventoryChanged.AddDynamic(this, &ULOTNHUDWidget::HandleInventoryChanged);
        }
        else
        {
            Player->InventoryComponent->OnInventoryChanged.RemoveDynamic(this, &ULOTNHUDWidget::HandleInventoryChanged);
        }
    }

    if (Player->InteractionComponent)
    {
        if (bBind)
        {
            Player->InteractionComponent->OnInteractionMessage.AddDynamic(this, &ULOTNHUDWidget::HandleInteractionMessage);
        }
        else
        {
            Player->InteractionComponent->OnInteractionMessage.RemoveDynamic(this, &ULOTNHUDWidget::HandleInteractionMessage);
        }
    }

    if (bBind)
    {
        BoundPlayer = Player;
    }
    else
    {
        BoundPlayer.Reset();
    }
}

void ULOTNHUDWidget::RefreshHUD()
{
    ALOTNCharacter* Player = Cast<ALOTNCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    if (!Player)
    {
        return;
    }

    if (const ULOTNPlayerStatsComponent* Stats = Player->PlayerStats)
    {
        if (HealthBar)
        {
            HealthBar->SetPercent(Stats->MaxHealth > 0.f ? Stats->Health / Stats->MaxHealth : 0.f);
        }
        if (StaminaBar)
        {
            StaminaBar->SetPercent(Stats->MaxStamina > 0.f ? Stats->Stamina / Stats->MaxStamina : 0.f);
        }
        if (GoldText)
        {
            GoldText->SetText(FText::Format(NSLOCTEXT("LOTN", "GoldFormat", "Gold: {0}"), FText::AsNumber(Stats->Gold)));
        }
        if (RenownText)
        {
            RenownText->SetText(FText::Format(NSLOCTEXT("LOTN", "RenownFormat", "Renown: {0}"), FText::AsNumber(Stats->Renown)));
        }
    }

    if (InventoryCountText && Player->InventoryComponent)
    {
        InventoryCountText->SetText(FText::Format(NSLOCTEXT("LOTN", "InventoryFormat", "Inventory: {0}"),
            FText::AsNumber(Player->InventoryComponent->GetTotalItemCount())));
    }

    if (QuestTitleText && QuestObjectiveText && QuestProgressText)
    {
        if (Player->QuestComponent && Player->QuestComponent->ActiveQuests.Num() > 0)
        {
            const FLOTNQuestEntry& Quest = Player->QuestComponent->ActiveQuests[0];
            QuestTitleText->SetText(Quest.Title);
            QuestObjectiveText->SetText(Quest.ObjectiveText);
            QuestProgressText->SetText(FText::Format(
                NSLOCTEXT("LOTN", "QuestProgressFormat", "{0} / {1}"),
                FText::AsNumber(Quest.CurrentProgress),
                FText::AsNumber(Quest.RequiredProgress)));
        }
        else
        {
            QuestTitleText->SetText(NSLOCTEXT("LOTN", "NoActiveQuest", "No active quest"));
            QuestObjectiveText->SetText(FText::GetEmpty());
            QuestProgressText->SetText(FText::GetEmpty());
        }
    }
}

void ULOTNHUDWidget::ShowDialogue(const FText& Message)
{
    if (DialogueText)
    {
        DialogueText->SetText(Message);
        if (DialoguePanel)
        {
            DialoguePanel->SetVisibility(ESlateVisibility::Visible);
        }
        else
        {
            DialogueText->SetVisibility(ESlateVisibility::Visible);
        }
    }
}

void ULOTNHUDWidget::HandleStatsChanged()
{
    RefreshHUD();
    RefreshWorldMapPanel();
}

void ULOTNHUDWidget::HandleQuestChanged()
{
    RefreshHUD();
}

void ULOTNHUDWidget::HandleInventoryChanged()
{
    RefreshHUD();
    RefreshInventoryPanel();
}

void ULOTNHUDWidget::ToggleInventoryPanel(bool bShow)
{
    if (InventoryPanel)
    {
        InventoryPanel->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
        if (bShow)
        {
            RefreshInventoryPanel();
        }
    }
}

void ULOTNHUDWidget::RefreshInventoryPanel()
{
    if (!InventoryList)
    {
        return;
    }

    InventoryList->ClearChildren();
    ALOTNCharacter* Player = Cast<ALOTNCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    if (!Player || !Player->InventoryComponent || Player->InventoryComponent->Items.Num() == 0)
    {
        UTextBlock* EmptyText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
        EmptyText->SetText(NSLOCTEXT("LOTN", "InventoryEmpty", "Your pack is empty."));
        EmptyText->SetColorAndOpacity(FSlateColor(FLinearColor(0.78f, 0.82f, 0.85f, 1.f)));
        EmptyText->SetFont(FCoreStyle::GetDefaultFontStyle("Regular", 13));
        InventoryList->AddChild(EmptyText);
        return;
    }

    for (const FLOTNInventoryItem& Item : Player->InventoryComponent->Items)
    {
        const bool bEquipped = Player->InventoryComponent->EquippedItemIds.Contains(Item.ItemId);
        const bool bCanEquip = Item.EquipmentSlot != ELOTNEquipmentSlot::None &&
            (Item.Category == ELOTNItemCategory::Apparel || Item.Category == ELOTNItemCategory::Trinket);

        if (bCanEquip)
        {
            if (ULOTNInventoryItemRowWidget* RowWidget = CreateWidget<ULOTNInventoryItemRowWidget>(
                GetOwningPlayer(), ULOTNInventoryItemRowWidget::StaticClass()))
            {
                RowWidget->SetupRow(Item, bEquipped);
                RowWidget->OnEquipRequested.AddDynamic(this, &ULOTNHUDWidget::HandleEquipItemRequested);
                InventoryList->AddChild(RowWidget);
            }
        }
        else
        {
            const TCHAR* CategoryLabel = TEXT("MATERIAL");
            switch (Item.Category)
            {
            case ELOTNItemCategory::Consumable: CategoryLabel = TEXT("CONSUMABLE"); break;
            case ELOTNItemCategory::Material: CategoryLabel = TEXT("MATERIAL"); break;
            case ELOTNItemCategory::Quest: CategoryLabel = TEXT("QUEST"); break;
            case ELOTNItemCategory::Apparel: CategoryLabel = TEXT("APPAREL"); break;
            case ELOTNItemCategory::Trinket: CategoryLabel = TEXT("TRINKET"); break;
            default: break;
            }

            UTextBlock* Row = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
            Row->SetText(FText::Format(
                NSLOCTEXT("LOTN", "InventoryItemFormat", "{0}  x{1}  [{2}]"),
                Item.DisplayName,
                FText::AsNumber(Item.Quantity),
                FText::FromString(CategoryLabel)));
            Row->SetColorAndOpacity(FSlateColor(FLinearColor(0.88f, 0.91f, 0.92f, 1.f)));
            Row->SetFont(FCoreStyle::GetDefaultFontStyle("Bold", 13));
            Row->SetAutoWrapText(true);
            InventoryList->AddChild(Row);
        }

        if (!Item.Description.IsEmpty())
        {
            UTextBlock* Description = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
            Description->SetText(Item.Description);
            Description->SetColorAndOpacity(FSlateColor(FLinearColor(0.62f, 0.68f, 0.72f, 1.f)));
            Description->SetFont(FCoreStyle::GetDefaultFontStyle("Regular", 11));
            Description->SetAutoWrapText(true);
            InventoryList->AddChild(Description);
        }
    }
}

void ULOTNHUDWidget::HandleInventoryToggleClicked()
{
    if (InventoryPanel)
    {
        ToggleInventoryPanel(InventoryPanel->GetVisibility() != ESlateVisibility::Visible);
    }
}

void ULOTNHUDWidget::HandleInventoryCloseClicked()
{
    ToggleInventoryPanel(false);
}

void ULOTNHUDWidget::HandleEquipItemRequested(FName ItemId)
{
    ALOTNCharacter* Player = Cast<ALOTNCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    if (!Player || !Player->InventoryComponent)
    {
        return;
    }

    const bool bCurrentlyEquipped = Player->InventoryComponent->EquippedItemIds.Contains(ItemId);
    const bool bChanged = bCurrentlyEquipped
        ? Player->InventoryComponent->UnequipItem(ItemId)
        : Player->InventoryComponent->EquipItem(ItemId);

    ShowDialogue(bChanged
        ? (bCurrentlyEquipped
            ? NSLOCTEXT("LOTN", "ItemUnequipped", "Item unequipped.")
            : NSLOCTEXT("LOTN", "ItemEquipped", "Item equipped."))
        : NSLOCTEXT("LOTN", "ItemEquipFailed", "This item cannot be equipped."));
}

void ULOTNHUDWidget::HandleInteractionMessage(const FText& Message)
{
    ShowDialogue(Message);
    RefreshHUD();
}

void ULOTNHUDWidget::HandleInteractClicked()
{
    if (ALOTNCharacter* Player = Cast<ALOTNCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
    {
        Player->InteractPressed();
    }
}

void ULOTNHUDWidget::HandleSprintPressed()
{
    if (ALOTNCharacter* Player = Cast<ALOTNCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
    {
        Player->StartSprint();
    }
}

void ULOTNHUDWidget::HandleSprintReleased()
{
    if (ALOTNCharacter* Player = Cast<ALOTNCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
    {
        Player->StopSprint();
    }
}

void ULOTNHUDWidget::HandleSaveClicked()
{
    if (UWorld* World = GetWorld())
    {
        if (UGameInstance* Instance = World->GetGameInstance())
        {
            if (ULOTNSaveSubsystem* SaveSystem = Instance->GetSubsystem<ULOTNSaveSubsystem>())
            {
                const bool bSaved = SaveSystem->SaveCurrentGame();
                ShowDialogue(bSaved
                    ? NSLOCTEXT("LOTN", "SaveSuccess", "Progress saved.")
                    : NSLOCTEXT("LOTN", "SaveFailed", "Could not save progress."));
            }
        }
    }
}

void ULOTNHUDWidget::HandleLoadClicked()
{
    if (UWorld* World = GetWorld())
    {
        if (UGameInstance* Instance = World->GetGameInstance())
        {
            if (ULOTNSaveSubsystem* SaveSystem = Instance->GetSubsystem<ULOTNSaveSubsystem>())
            {
                const bool bLoaded = SaveSystem->LoadCurrentGame();
                ShowDialogue(bLoaded
                    ? NSLOCTEXT("LOTN", "LoadSuccess", "Progress loaded.")
                    : NSLOCTEXT("LOTN", "LoadFailed", "No compatible save was found in this location."));
                RefreshHUD();
            }
        }
    }
}
