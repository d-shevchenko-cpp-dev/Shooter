#include "Weapon/Configuration/STUWeaponConfiguration.h"
#include "Engine/Engine.h"

DEFINE_LOG_CATEGORY_STATIC(LogWeaponConfiguration, All, All);

USTUWeaponConfiguration::USTUWeaponConfiguration()
{
    MaxRange = 2000.0f;
    ShotDelay = 0.1f;
    BulletSpread = 1.5f;
    DamageAmount = 10.0f;
    HeadshotMultiplier = 2.0f;
    HeadBoneName = TEXT("b_head");
    NeckBoneName = TEXT("b_Neck");
    DamageType = nullptr;
    bEnableHealthSpreadModifier = false;
    HealthSpreadThreshold = 0.3f;
    MaxHealthSpreadMultiplier = 3.0f;
}

bool USTUWeaponConfiguration::IsHeadshot(const FName& HitBoneName) const
{
    return HitBoneName == HeadBoneName || HitBoneName == NeckBoneName;
}

void USTUWeaponConfiguration::ClampValue(float& Value, float MinValue, float MaxValue, const FString& ValueName) const
{
    if (Value < MinValue || Value > MaxValue)
    {
        UE_LOG(LogWeaponConfiguration,
            Warning,
            TEXT("Invalid %s: %f. Clamping to valid range [%f, %f]"),
            *ValueName,
            Value,
            MinValue,
            MaxValue);
        Value = FMath::Clamp(Value, MinValue, MaxValue);
    }
}
