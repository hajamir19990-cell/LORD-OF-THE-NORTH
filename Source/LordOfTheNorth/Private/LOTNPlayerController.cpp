#include "LOTNPlayerController.h"
#include "LOTNHUDWidget.h"

ALOTNPlayerController::ALOTNPlayerController()
{
    bShowMouseCursor = false;
    bEnableTouchEvents = true;
    bEnableClickEvents = true;
    HUDWidgetClass = ULOTNHUDWidget::StaticClass();
}

void ALOTNPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (HUDWidgetClass)
    {
        HUDWidget = CreateWidget<ULOTNHUDWidget>(this, HUDWidgetClass);
        if (HUDWidget)
        {
            HUDWidget->AddToViewport();
        }
    }

    // Keep gameplay input active while allowing the native HUD to receive touch/clicks.
    FInputModeGameAndUI InputMode;
    InputMode.SetHideCursorDuringCapture(false);
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    SetInputMode(InputMode);
}
