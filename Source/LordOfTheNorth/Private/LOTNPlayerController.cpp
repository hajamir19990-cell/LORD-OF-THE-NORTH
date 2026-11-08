#include "LOTNPlayerController.h"
#include "LOTNHUDWidget.h"

ALOTNPlayerController::ALOTNPlayerController()
{
    bShowMouseCursor = false;
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

    FInputModeGameOnly InputMode;
    SetInputMode(InputMode);
}
