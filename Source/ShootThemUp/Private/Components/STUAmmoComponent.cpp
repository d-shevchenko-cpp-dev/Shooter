#include "Components/STUAmmoComponent.h"
#include "Engine/Engine.h"

USTUAmmoComponent::USTUAmmoComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void USTUAmmoComponent::Initialize(const FAmmoData& InitialAmmo)
{
    DefaultAmmo = InitialAmmo;
    CurrentAmmo = InitialAmmo;
}

bool USTUAmmoComponent::TryDecreaseAmmo()
{
    if (IsAmmoEmpty())
    {
        return false;
    }

    CurrentAmmo.BulletsInClip--;

    if (IsClipEmpty() && !IsAmmoEmpty())
    {
        OnClipEmpty.Broadcast();
    }

    return true;
}

bool USTUAmmoComponent::TryReload()
{
    if (!CanReload())
    {
        return false;
    }

    auto CurrentBullets = CurrentAmmo.BulletsInClip;
    CurrentAmmo.BulletsInClip = DefaultAmmo.BulletsInClip;

    if (!CurrentAmmo.InfiniteCheat)
    {
        CurrentAmmo.Bullets = CurrentAmmo.Bullets - CurrentAmmo.BulletsInClip + CurrentBullets;
    }

    return true;
}

bool USTUAmmoComponent::IsClipEmpty() const
{
    return CurrentAmmo.BulletsInClip == 0;
}

bool USTUAmmoComponent::IsAmmoEmpty() const
{
    return IsClipEmpty() && !CurrentAmmo.InfiniteCheat && CurrentAmmo.Bullets == 0;
}

bool USTUAmmoComponent::CanReload() const
{
    if (CurrentAmmo.InfiniteCheat)
    {
        return true;
    }

    if (CurrentAmmo.Bullets == 0 || CurrentAmmo.BulletsInClip == DefaultAmmo.BulletsInClip)
    {
        return false;
    }

    return CurrentAmmo.Bullets > 0;
}
