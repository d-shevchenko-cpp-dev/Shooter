// Игра ShootThemUp. Все права защищены.

#include "Weapon/STUWeaponDebugManager.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

USTUWeaponDebugManager::USTUWeaponDebugManager()
{
    DebugSettings = FSTUWeaponDebugSettings();
}

void USTUWeaponDebugManager::Initialize(const FSTUWeaponDebugSettings& Settings)
{
    DebugSettings = Settings;
}

bool USTUWeaponDebugManager::IsDebugEnabledForCategory(ESTUWeaponDebugCategory Category) const
{
    if (!DebugSettings.bEnableDebug)
    {
        return false;
    }

    const int32 CategoryInt = static_cast<int32>(Category);
    return (DebugSettings.DebugCategories & CategoryInt) != 0;
}

void USTUWeaponDebugManager::SetDebugEnabledForCategory(ESTUWeaponDebugCategory Category, bool bEnabled)
{
    const int32 CategoryInt = static_cast<int32>(Category);
    
    if (bEnabled)
    {
        DebugSettings.DebugCategories |= CategoryInt;
    }
    else
    {
        DebugSettings.DebugCategories &= ~CategoryInt;
    }
}

void USTUWeaponDebugManager::SetDebugEnabled(bool bEnabled)
{
    DebugSettings.bEnableDebug = bEnabled;
}

void USTUWeaponDebugManager::UpdateDebugSettings(const FSTUWeaponDebugSettings& Settings)
{
    DebugSettings = Settings;
}

void USTUWeaponDebugManager::DrawTraceLine(UWorld* World, const FVector& Start, const FVector& End) const
{
    if (!IsDebugEnabledForCategory(ESTUWeaponDebugCategory::Trace) || !World)
    {
        return;
    }

    DrawDebugLine(World, 
        Start, 
        End, 
        DebugSettings.TraceLineColor, 
        false, 
        DebugSettings.DebugDuration, 
        0, 
        DebugSettings.TraceLineThickness);
}

void USTUWeaponDebugManager::DrawHitSphere(UWorld* World, const FVector& Location) const
{
    if (!IsDebugEnabledForCategory(ESTUWeaponDebugCategory::Hit) || !World)
    {
        return;
    }

    DrawDebugSphere(World,
        Location,
        DebugSettings.HitSphereRadius,
        24,
        DebugSettings.HitSphereColor,
        false,
        DebugSettings.DebugDuration);
}

void USTUWeaponDebugManager::DrawCameraLine(UWorld* World, const FVector& Start, const FVector& End) const
{
    if (!IsDebugEnabledForCategory(ESTUWeaponDebugCategory::Camera) || !World)
    {
        return;
    }

    DrawDebugLine(World,
        Start,
        End,
        DebugSettings.CameraLineColor,
        false,
        DebugSettings.DebugDuration,
        0,
        2.0f);
}

void USTUWeaponDebugManager::DrawSpreadSphere(UWorld* World, const FVector& Location) const
{
    if (!IsDebugEnabledForCategory(ESTUWeaponDebugCategory::Spread) || !World)
    {
        return;
    }

    DrawDebugSphere(World,
        Location,
        DebugSettings.SpreadSphereRadius,
        8,
        DebugSettings.SpreadColor,
        false,
        DebugSettings.DebugDuration);
}
