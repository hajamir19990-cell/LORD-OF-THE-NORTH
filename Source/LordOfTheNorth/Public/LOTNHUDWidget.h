#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LOTNHUDWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UButton;

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
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

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
    TObjectPtr<UButton> InteractButton;

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
    void HandleInteractionMessage(const FText& Message);

    UFUNCTION()
    void HandleInteractClicked();

    UFUNCTION()
    void HandleSaveClicked();

    UFUNCTION()
    void HandleLoadClicked();

    void BindPlayerEvents(bool bBind);
};
