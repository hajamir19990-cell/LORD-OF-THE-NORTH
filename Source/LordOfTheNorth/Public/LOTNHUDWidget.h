#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LOTNHUDWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UButton;
class UBorder;
class UScrollBox;
class ALOTNCharacter;

UCLASS()
class LORDOFTHENORTH_API ULOTNHUDWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="LOTN|HUD")
    void RefreshHUD();

    UFUNCTION(BlueprintCallable, Category="LOTN|HUD")
    void ShowDialogue(const FText& Message);

protected:
    virtual void NativeOnInitialized() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    UPROPERTY(meta=(BindWidgetOptional))
    TObjectPtr<UProgressBar> HealthBar;

    UPROPERTY(meta=(BindWidgetOptional))
    TObjectPtr<UProgressBar> StaminaBar;

    UPROPERTY(meta=(BindWidgetOptional))
    TObjectPtr<UTextBlock> GoldText;

    UPROPERTY(meta=(BindWidgetOptional))
    TObjectPtr<UTextBlock> RenownText;

    UPROPERTY(meta=(BindWidgetOptional))
    TObjectPtr<UTextBlock> InventoryCountText;

    UPROPERTY(meta=(BindWidgetOptional))
    TObjectPtr<UTextBlock> QuestTitleText;

    UPROPERTY(meta=(BindWidgetOptional))
    TObjectPtr<UTextBlock> QuestObjectiveText;

    UPROPERTY(meta=(BindWidgetOptional))
    TObjectPtr<UTextBlock> QuestProgressText;

    UPROPERTY(meta=(BindWidgetOptional))
    TObjectPtr<UTextBlock> DialogueText;

    UPROPERTY(meta=(BindWidgetOptional))
    TObjectPtr<UBorder> DialoguePanel;

    UPROPERTY(meta=(BindWidgetOptional))
    TObjectPtr<UBorder> InteractionPromptPanel;

    UPROPERTY(meta=(BindWidgetOptional))
    TObjectPtr<UTextBlock> InteractionPromptText;

    UPROPERTY(meta=(BindWidgetOptional))
    TObjectPtr<UButton> InteractButton;

    UPROPERTY(meta=(BindWidgetOptional))
    TObjectPtr<UButton> SprintButton;

    UPROPERTY(meta=(BindWidgetOptional))
    TObjectPtr<UButton> InventoryButton;

    UPROPERTY(meta=(BindWidgetOptional))
    TObjectPtr<UButton> InventoryCloseButton;

    UPROPERTY(meta=(BindWidgetOptional))
    TObjectPtr<UBorder> InventoryPanel;

    UPROPERTY(meta=(BindWidgetOptional))
    TObjectPtr<UScrollBox> InventoryList;

    UPROPERTY(meta=(BindWidgetOptional))
    TObjectPtr<UButton> SaveButton;

    UPROPERTY(meta=(BindWidgetOptional))
    TObjectPtr<UButton> LoadButton;

private:
    UFUNCTION()
    void HandleStatsChanged();

    UFUNCTION()
    void HandleQuestChanged();

    UFUNCTION()
    void HandleInventoryChanged();

    UFUNCTION()
    void HandleInventoryToggleClicked();

    UFUNCTION()
    void HandleInventoryCloseClicked();

    UFUNCTION()
    void HandleInteractionMessage(const FText& Message);

    UFUNCTION()
    void HandleInteractClicked();

    UFUNCTION()
    void HandleSprintPressed();

    UFUNCTION()
    void HandleSprintReleased();

    UFUNCTION()
    void HandleSaveClicked();

    UFUNCTION()
    void HandleLoadClicked();

    void BindPlayerEvents(bool bBind);
    void BuildNativeLayout();
    void RefreshInventoryPanel();
    void ToggleInventoryPanel(bool bShow);

    FText CachedInteractionPrompt;
    float PromptRefreshAccumulator = 0.f;
    float PromptRefreshInterval = 0.15f;
    TWeakObjectPtr<ALOTNCharacter> BoundPlayer;
};
