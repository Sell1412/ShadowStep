// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.


#include "GoalViewIcon.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"

void UGoalViewIcon::UpdateScreenPos(APlayerController* PlayerController,FVector GoalPosition)
{
    if (!PlayerController) return;

    float ViewportScale = UWidgetLayoutLibrary::GetViewportScale(this);
    int32 SizeX, SizeY;
    PlayerController->GetViewportSize(SizeX, SizeY);
    FVector2D Center(SizeX * 0.5f / ViewportScale, SizeY * 0.5f / ViewportScale);

    // --- Always calculate direction via camera axes (works even behind camera) ---
    FVector CameraLoc;
    FRotator CameraRot;
    PlayerController->GetPlayerViewPoint(CameraLoc, CameraRot);

    FVector DirectionToGoal = (GoalPosition - CameraLoc).GetSafeNormal();
    FVector CamRight = FRotationMatrix(CameraRot).GetScaledAxis(EAxis::Y);
    FVector CamUp = FRotationMatrix(CameraRot).GetScaledAxis(EAxis::Z);
    FVector CamForward = FRotationMatrix(CameraRot).GetScaledAxis(EAxis::X);

    float DotRight = FVector::DotProduct(DirectionToGoal, CamRight);
    float DotUp = FVector::DotProduct(DirectionToGoal, CamUp);
    float DotForward = FVector::DotProduct(DirectionToGoal, CamForward);

    FVector2D FinalPos;

    // Try screen projection only when goal is in front of camera
    FVector2D ScreenPosition;
    bool bIsInFront = DotForward > 0.f;
    bool bProjected = bIsInFront &&
        PlayerController->ProjectWorldLocationToScreen(GoalPosition, ScreenPosition, true);

    if (bProjected)
    {
        FVector2D ScaledPos = ScreenPosition / ViewportScale;

        // Check if actually within screen bounds
        float EdgeX = Center.X - 60.f;
        float EdgeY = Center.Y - 60.f;
        bool bOnScreen = FMath::Abs(ScaledPos.X - Center.X) <= EdgeX &&
            FMath::Abs(ScaledPos.Y - Center.Y) <= EdgeY;

        if (bOnScreen)
        {
            FinalPos = ScaledPos;
        }
        else
        {
            // On screen projection worked but goal is near edge — clamp to border
            FVector2D Dir = FVector2D(DotRight, -DotUp).GetSafeNormal();
            float Scale = FMath::Min(EdgeX / FMath::Abs(Dir.X + 0.0001f),
                EdgeY / FMath::Abs(Dir.Y + 0.0001f));
            FinalPos = Center + Dir * Scale;
        }
    }
    else
    {
        // Behind camera or projection failed — derive edge pos from camera axes only
        // When behind, flip DotRight so the icon goes to the correct horizontal edge
        FVector2D Dir = FVector2D(
            bIsInFront ? DotRight : -DotRight,
            -DotUp
        ).GetSafeNormal();

        float EdgeX = Center.X - 60.f;
        float EdgeY = Center.Y - 60.f;
        float Scale = FMath::Min(EdgeX / FMath::Abs(Dir.X + 0.0001f),
            EdgeY / FMath::Abs(Dir.Y + 0.0001f));
        FinalPos = Center + Dir * Scale;
    }

    // Set Position in Canvas Panel
    UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Slot);
    if (CanvasSlot)
    {
        FVector2D WidgetSize = GetDesiredSize();
        CanvasSlot->SetPosition(FinalPos - WidgetSize * 0.5f);
    }
}
