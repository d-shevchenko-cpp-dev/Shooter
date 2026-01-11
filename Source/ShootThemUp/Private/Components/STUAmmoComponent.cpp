#include "Components/STUAmmoComponent.h"
#include "Engine/Engine.h"

DEFINE_LOG_CATEGORY_STATIC(LogAmmoComponent, All, All);

USTUAmmoComponent::USTUAmmoComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void USTUAmmoComponent::Initialize(const FAmmoData& InitialAmmo)
{
    DefaultAmmo = InitialAmmo;
    CurrentAmmo = InitialAmmo;

    UE_LOG(LogAmmoComponent,
        Log,
        TEXT("Ammo initialized: Bullets=%d, Clips=%d, Infinite=%d"),
        CurrentAmmo.Bullets,
        CurrentAmmo.Clips,
        CurrentAmmo.Infinite);
}

bool USTUAmmoComponent::TryDecreaseAmmo()
{
    if (IsAmmoEmpty())
    {
        UE_LOG(LogAmmoComponent, Warning, TEXT("Cannot decrease ammo: ammo is empty"));
        return false;
    }

    CurrentAmmo.Bullets--;
    LogAmmo();

    // Автоматическая перезарядка если обойма пуста
    if (IsClipEmpty() && !IsAmmoEmpty())
    {
        TryReload();
    }

    return true;
}

bool USTUAmmoComponent::TryReload()
{
    if (!CanReload())
    {
        UE_LOG(LogAmmoComponent, Warning, TEXT("Cannot reload: no clips available or clip is full"));
        return false;
    }

    CurrentAmmo.Bullets = DefaultAmmo.Bullets;

    if (!CurrentAmmo.Infinite)
    {
        CurrentAmmo.Clips--;
    }

    UE_LOG(LogAmmoComponent, Display, TEXT("Reload completed"));
    LogAmmo();

    return true;
}

bool USTUAmmoComponent::IsClipEmpty() const
{
    return CurrentAmmo.Bullets == 0;
}

bool USTUAmmoComponent::IsAmmoEmpty() const
{
    return IsClipEmpty() && !CurrentAmmo.Infinite && CurrentAmmo.Clips == 0;
}

bool USTUAmmoComponent::CanReload() const
{
    if (CurrentAmmo.Infinite)
    {
        return true;
    }

    if (CurrentAmmo.Clips == 0 || CurrentAmmo.Bullets == DefaultAmmo.Bullets)
    {
        return false;
    }

    return CurrentAmmo.Clips > 0;
}

void USTUAmmoComponent::LogAmmo() const
{
    FString AmmoInfo = FString::Printf(TEXT("Ammo: %d / %s"),
        CurrentAmmo.Bullets,
        CurrentAmmo.Infinite ? TEXT("∞") : *FString::FromInt(CurrentAmmo.Clips));

    UE_LOG(LogAmmoComponent, Display, TEXT("%s"), *AmmoInfo);
}
