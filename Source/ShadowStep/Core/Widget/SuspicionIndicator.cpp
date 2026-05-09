// Copyright (c) 2026 Core Memory Entertainment GbR. All rights reserved.


#include "SuspicionIndicator.h"
#include "Blueprint/WidgetLayoutLibrary.h"


float USuspicionIndicator::GetEnemyRotationAngle(FVector EnemyLocation) const
{
// Calulaute Enemy Location in SCreenSpace for Rotation of the Widget

	APlayerController* PlayerController = GetOwningPlayer();
	FVector2D ScreenLocation;
	PlayerController->ProjectWorldLocationToScreen(EnemyLocation, ScreenLocation);

	// Calculate the center of the screen
	int32 ViewportSizeX, ViewportSizeY;
	PlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);
	FVector2D ScreenCenter(ViewportSizeX / 2.0f, ViewportSizeY / 2.0f);

	// Calculate the angle between the center of the screen and the enemy location
	FVector2D Direction = ScreenLocation - ScreenCenter;
	float Angle = FMath::Atan2(Direction.Y, Direction.X) * (180.0f / PI);
	return Angle;
}


void USuspicionIndicator::SetWidgetOffestAndRotation(float OffsetDistance,float RotationToEnemyInScreenSpace,FVector EnemyWorldPosition)
{

    APlayerController* PC = GetOwningPlayer();
    if (!PC) return;

    FVector2D ViewportSize;
    if (GEngine && GEngine->GameViewport)
        GEngine->GameViewport->GetViewportSize(ViewportSize);

    float ViewportScale = UWidgetLayoutLibrary::GetViewportScale(this);
    FVector2D ScreenCenter = (ViewportSize * 0.5f) / ViewportScale;

    // --- World space angle calculation ---
    // Get camera transform
    FVector CameraLocation;
    FRotator CameraRotation;
    PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

    // Head position
    FVector HeadPosition = EnemyWorldPosition + FVector(0.f, 0.f, 200.f);

    // Direction from camera to enemy in world space
    FVector DirectionToEnemy = (HeadPosition - CameraLocation).GetSafeNormal();

    // Project onto camera's right and up axes
    // This gives us a 2D offset that works even when enemy is off-screen
    FVector CamRight = FRotationMatrix(CameraRotation).GetScaledAxis(EAxis::Y);
    FVector CamUp = FRotationMatrix(CameraRotation).GetScaledAxis(EAxis::Z);

    float DotRight = FVector::DotProduct(DirectionToEnemy, CamRight);
    float DotUp = FVector::DotProduct(DirectionToEnemy, CamUp);

    // Atan2 in screen space: X = right, Y = down (flip DotUp for screen coords)
    float AngleDeg = FMath::RadiansToDegrees(FMath::Atan2(-DotUp, DotRight));

    // --- Fixed offset position on circle ---
    float AngleRad = FMath::DegreesToRadians(AngleDeg);
    FVector2D WidgetPosition;
    WidgetPosition.X = ScreenCenter.X + FMath::Cos(AngleRad) * OffsetDistance;
    WidgetPosition.Y = ScreenCenter.Y + FMath::Sin(AngleRad) * OffsetDistance;

    FVector2D WidgetSize = GetDesiredSize();
    SetPositionInViewport(WidgetPosition - (WidgetSize * 0.5f), false);

    // --- Rotate widget to point toward enemy ---
    FWidgetTransform Transform;
    Transform.Angle = AngleDeg + 90.0f;
	float WorldDistance = FVector::Dist(GetOwningPlayerPawn()->GetActorLocation(), EnemyWorldPosition);

	float NormalizedWorldDist = FMath::Clamp(WorldDistance / maxWorldDistance, 0.0f, 1.0f);
	float FinalScale = FMath::Lerp(1.5f, 0.5f, NormalizedWorldDist);

	Transform.Scale = FVector2D(FinalScale, FinalScale);
    Transform.Translation = FVector2D::ZeroVector;
    Transform.Shear = FVector2D::ZeroVector;
    SetRenderTransform(Transform);

}

//FVector2D ViewportSize;
//if (GEngine && GEngine->GameViewport)
//{
//	GEngine->GameViewport->GetViewportSize(ViewportSize);
//}
//float ViewportScale = 1.0f;
//if (GEngine && GEngine->GameViewport)
//{
//	GEngine->GameViewport->GetViewportSize(ViewportSize);
//	ViewportScale = UWidgetLayoutLibrary::GetViewportScale(this);
//}

//   // ScreenCenter in Widget-Space (nicht in Screen-Pixel-Space)
//   FVector2D ScreenCenter = (ViewportSize * 0.5f) / ViewportScale;

//   // --- 2. Gegner auf Screen projizieren ---
//   APlayerController* PC = GetOwningPlayer();
//   if (!PC) return;

   //FVector2D EnemyScreenPos;
   //bool bIsOnScreen = PC->ProjectWorldLocationToScreen(
   //	EnemyWorldPosition, EnemyScreenPos, true);

//   EnemyScreenPos /= ViewportScale; 

//   FVector2D DeltaToEnemy = EnemyScreenPos - ScreenCenter; 
//   float ScreenDistance = DeltaToEnemy.Size();

//   float NormalizedDist = FMath::Clamp(ScreenDistance / maxScreenDist, 0.0f, 1.0f);

//   // Jetzt Lerp: nah = kleiner Offset, weit = großer Offset
//   float FinalOffset = FMath::Lerp(OffsetDistanceClose, OffsetDistanceFar, NormalizedDist);

//   // --- 4. Position auf dem Offset-Kreis berechnen ---

//   float AngleRad = FMath::DegreesToRadians(RotationToEnemyInScreenSpace);

//   FVector2D WidgetPosition;
//   WidgetPosition.X = ScreenCenter.X + FMath::Cos(AngleRad) * FinalOffset;
//   WidgetPosition.Y = ScreenCenter.Y + FMath::Sin(AngleRad) * FinalOffset;

//   // --- 5. Widget-Größe abziehen damit es zentriert auf dem Kreispunkt sitzt ---

   //FVector2D WidgetSize = GetDesiredSize();

//   FVector2D FinalPosition = WidgetPosition - (WidgetSize * 0.5f);
//   SetPositionInViewport(FinalPosition, false);

//   // --- 6. Widget rotieren damit die Bar zum Gegner zeigt ---

//   float FinalAngle = RotationToEnemyInScreenSpace + 90.0f;

//   FWidgetTransform Transform;
//   Transform.Angle = FinalAngle;

   //// Change also the Scale to a max 0.5 if the Enemy is far away and to 1.0 if the Enemy is close.
   //// Based on the distance to the enemy in the World Position. The further the Enemy is away, the smaller the Widget. The closer the Enemy is, the bigger the Widget. This is to give the Player a better visual feedback of how far away the Enemy is.
   //float WorldDistance = FVector::Dist(GetOwningPlayerPawn()->GetActorLocation(), EnemyWorldPosition);

   //float NormalizedWorldDist = FMath::Clamp(WorldDistance / maxWorldDistance, 0.0f, 1.0f);
   //float FinalScale = FMath::Lerp(1.0f, 0.3f, NormalizedWorldDist);

   //Transform.Scale = FVector2D(FinalScale, FinalScale);

//   Transform.Translation = FVector2D::ZeroVector;
//   Transform.Shear = FVector2D::ZeroVector;
//   SetRenderTransform(Transform);

void USuspicionIndicator::UpdateOpacity(float SusValue, float DeltaTime)
{
    float TargetOpacity = (SusValue > 0.01f) ? 1.0f : 0.0f;

	float FadeSpeed = (TargetOpacity > currentOpacity) ? 5.0f : 2.0f;

    currentOpacity = FMath::FInterpTo(currentOpacity, TargetOpacity, DeltaTime, FadeSpeed);
    
    // Auf Widget anwenden
    SetRenderOpacity(currentOpacity);

    // Widget komplett verstecken wenn unsichtbar (spart Performance)
    if (currentOpacity < 0.01f)
    {
        SetVisibility(ESlateVisibility::Hidden);
    }
    else
    {
        SetVisibility(ESlateVisibility::HitTestInvisible);
    }

}


