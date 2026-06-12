// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.


#include "GoalViewIcon.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"

void UGoalViewIcon::UpdateScreenPos(APlayerController* PlayerController,FVector GoalPosition)
{
	//FVector2D ScreenPosition;
	//bool isInScreen = PlayerController->ProjectWorldLocationToScreen(GoalPosition, ScreenPosition,true);

	//float ViewportScale = UWidgetLayoutLibrary::GetViewportScale(this);
	//FVector2D ScaledPos = ScreenPosition / ViewportScale;

 //   if (!isInScreen)
 //   {
 //       int32 SizeX, SizeY;
 //       PlayerController->GetViewportSize(SizeX, SizeY);
 //       FVector2D Center(SizeX * 0.5f / ViewportScale, SizeY * 0.5f / ViewportScale);
 //       FVector2D Dir = (ScaledPos - Center).GetSafeNormal();
 //       float EdgeX = SizeX * 0.5f / ViewportScale - 60.f;
 //       float EdgeY = SizeY * 0.5f / ViewportScale - 60.f;
 //       float Scale = FMath::Min(EdgeX / FMath::Abs(Dir.X + 0.0001f),
 //           EdgeY / FMath::Abs(Dir.Y + 0.0001f));
 //       ScaledPos = Center + Dir * Scale;
 //   }

 //   // Cast the slot to CanvasPanelSlot and set position directly
 //   UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Slot);
 //   if (CanvasSlot)
 //   {
 //       FVector2D WidgetSize = GetDesiredSize();
 //       CanvasSlot->SetPosition(ScaledPos - WidgetSize * 0.5f);
 //   }
    if (!PlayerController) return;

    float ViewportScale = UWidgetLayoutLibrary::GetViewportScale(this);
    int32 SizeX, SizeY;
    PlayerController->GetViewportSize(SizeX, SizeY);
    FVector2D Center(SizeX * 0.5f / ViewportScale, SizeY * 0.5f / ViewportScale);

    FVector2D ScreenPosition;
    bool bIsInScreen = PlayerController->ProjectWorldLocationToScreen(GoalPosition, ScreenPosition, true);
    FVector2D ScaledPos = ScreenPosition / ViewportScale;

    if (!bIsInScreen)
    {
        // Use camera-space projection instead of the stale screen coords
        FVector CameraLoc;
        FRotator CameraRot;
        PlayerController->GetPlayerViewPoint(CameraLoc, CameraRot);

        FVector DirectionToGoal = (GoalPosition - CameraLoc).GetSafeNormal();
        FVector CamRight = FRotationMatrix(CameraRot).GetScaledAxis(EAxis::Y);
        FVector CamUp = FRotationMatrix(CameraRot).GetScaledAxis(EAxis::Z);

        float DotRight = FVector::DotProduct(DirectionToGoal, CamRight);
        float DotUp = FVector::DotProduct(DirectionToGoal, CamUp);

        // DotUp flipped because screen Y goes downward
        FVector2D Dir = FVector2D(DotRight, -DotUp).GetSafeNormal();

        float EdgeX = Center.X - 60.f;
        float EdgeY = Center.Y - 60.f;
        float Scale = FMath::Min(EdgeX / FMath::Abs(Dir.X + 0.0001f),
            EdgeY / FMath::Abs(Dir.Y + 0.0001f));
        ScaledPos = Center + Dir * Scale;
    }

    UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Slot);
    if (CanvasSlot)
    {
        FVector2D WidgetSize = GetDesiredSize();
        CanvasSlot->SetPosition(ScaledPos - WidgetSize * 0.5f);
    }
}
