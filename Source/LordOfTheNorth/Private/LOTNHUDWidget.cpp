#include "LOTNHUDWidget.h"
#include "LOTNCharacter.h"
#include "LOTNPlayerStatsComponent.h"
#include "LOTNQuestComponent.h"
#include "LOTNInventoryComponent.h"
#include "LOTNInteractionComponent.h"
#include "LOTNSaveSubsystem.h"
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
    SaveButton = AddButton(TEXT("SaveButton"), TEXT("SAVE"),
        FAnchors(0.f, 1.f, 0.f, 1.f), FVector2D(0.f, 1.f), FVector2D(14.f, -112.f), FVector2D(50.f, 44.f));
    LoadButton = AddButton(TEXT("LoadButton"), TEXT("LOAD"),
        FAnchors(0.f, 1.f, 0.f, 1.f), FVector2D(0.f, 1.f), FVector2D(70.f, -112.f), FVector2D(50.f, 44.f));
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
}

void ULOTNHUDWidget::HandleQuestChanged()
{
    RefreshHUD();
}

void ULOTNHUDWidget::HandleInventoryChanged()
{
    RefreshHUD();
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
