#include "UI/STUPlayerHUDWidget.h"
#include "STUAmmoComponent.h"
#include "STUBaseWeapon.h"
#include "STUWeaponComponent.h"
#include "Components/STUHealthComponent.h"

float USTUPlayerHUDWidget::GetHealthPercent() const
{
    const auto Player = GetOwningPlayerPawn();
    if (!Player)
    {
        return 0.0f;
    }

    const auto Component = Player->GetComponentByClass(USTUHealthComponent::StaticClass());
    const auto HealthComponent = Cast<USTUHealthComponent>(Component);
    if (!HealthComponent)
    {
        return 0.0f;
    }

    return HealthComponent->GetHealthPercent();
}

bool USTUPlayerHUDWidget::GetWeaponUIData(FWeaponUIData& UIData) const
{
    const auto Player = GetOwningPlayerPawn();
    if (!Player)
    {
        return false;
    }

    const auto Component = Player->GetComponentByClass(USTUWeaponComponent::StaticClass());
    const auto WeaponComponent = Cast<USTUWeaponComponent>(Component);
    if (!WeaponComponent)
    {
        return false;
    }

    return WeaponComponent->GetWeaponUIData(UIData);
}

FString USTUPlayerHUDWidget::GetCurrentAmmo() const
{
    const auto Player = GetOwningPlayerPawn();
    if (!Player)
    {
        return "N / N";
    }

    const auto Component = Player->GetComponentByClass(USTUWeaponComponent::StaticClass());
    const auto WeaponComponent = Cast<USTUWeaponComponent>(Component);
    if (!WeaponComponent)
    {
        return "N / N";
    }

    const auto CurrentWeapon = WeaponComponent->GetCurrentWeapon();
    check(CurrentWeapon);

    auto CurrentAmmo = CurrentWeapon->GetAmmoComponent()->GetCurrentAmmo();

    return FString::Printf(TEXT("%d / %d"), CurrentAmmo.Bullets, CurrentAmmo.Clips);
}
