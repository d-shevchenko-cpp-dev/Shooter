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
        TEXT("Ammo initialized: BulletsInClip=%d, Bullets=%d, InfiniteCheat=%d"),
        CurrentAmmo.BulletsInClip,
        CurrentAmmo.Bullets,
        CurrentAmmo.InfiniteCheat);
}

bool USTUAmmoComponent::TryDecreaseAmmo()
{
    if (IsAmmoEmpty())
    {
        return false;
    }

    CurrentAmmo.BulletsInClip--;
    LogAmmo();

    // Автоматическая перезарядка если обойма пуста
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

    UE_LOG(LogAmmoComponent, Display, TEXT("Reload completed"));
    LogAmmo();

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

void USTUAmmoComponent::LogAmmo() const
{
    FString AmmoInfo = FString::Printf(TEXT("Ammo: %d / %s"),
        CurrentAmmo.BulletsInClip,
        CurrentAmmo.InfiniteCheat ? TEXT("∞") : *FString::FromInt(CurrentAmmo.Bullets));

    UE_LOG(LogAmmoComponent, Display, TEXT("%s"), *AmmoInfo);
}
