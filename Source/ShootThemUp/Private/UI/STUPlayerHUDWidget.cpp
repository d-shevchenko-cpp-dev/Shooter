#include "UI/STUPlayerHUDWidget.h"
#include "STUAmmoComponent.h"
#include "STUBaseWeapon.h"
#include "STUWeaponComponent.h"
#include "Components/STUHealthComponent.h"
#include "Utils/STUUtils.h"

float USTUPlayerHUDWidget::GetHealthPercent() const
{
    if (const auto HeathComponent = GetHeathComponent())
    {
        return HeathComponent->GetHealthPercent();
    }
    return 0.0f;
}

bool USTUPlayerHUDWidget::GetWeaponUIData(FWeaponUIData& UIData) const
{
    if (const auto WeaponComponent = GetWeaponComponent())
    {
        return WeaponComponent->GetWeaponUIData(UIData);
    }
    return nullptr;
}

FString USTUPlayerHUDWidget::GetCurrentAmmo() const
{
    if (const auto WeaponComponent = GetWeaponComponent())
    {
        const auto CurrentWeapon = WeaponComponent->GetCurrentWeapon();
        if (!CurrentWeapon)
        {
            return "";
        }
        auto CurrentAmmo = CurrentWeapon->GetAmmoComponent()->GetCurrentAmmo();

        return FString::Printf(TEXT("%d / %d"), CurrentAmmo.BulletsInClip, CurrentAmmo.Bullets);
    }
    return "";
}

bool USTUPlayerHUDWidget::IsPlayerAlive() const
{
    const auto HealthComponent = GetHeathComponent();
    return HealthComponent && !HealthComponent->IsDead();
}

bool USTUPlayerHUDWidget::IsSpectatingMode() const
{
    const auto Controller = GetOwningPlayer();
    return Controller && Controller->GetStateName() == NAME_Spectating;
}

USTUWeaponComponent* USTUPlayerHUDWidget::GetWeaponComponent() const
{
    return STUUtils::GetSTUPlayerComponent<USTUWeaponComponent>(GetOwningPlayerPawn());
}

USTUHealthComponent* USTUPlayerHUDWidget::GetHeathComponent() const
{
    return STUUtils::GetSTUPlayerComponent<USTUHealthComponent>(GetOwningPlayerPawn());
}
