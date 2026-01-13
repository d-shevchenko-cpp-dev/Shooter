#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "STUWeaponConfiguration.generated.h"

class UDamageType;

UCLASS(BlueprintType, Blueprintable)
class SHOOTTHEMUP_API USTUWeaponConfiguration : public UObject
{
    GENERATED_BODY()

public:
    USTUWeaponConfiguration();

    /** Максимальная дальность стрельбы */
    UPROPERTY(EditDefaultsOnly,
        BlueprintReadWrite,
        Category = "Weapon Settings",
        meta = (ClampMin = "100.0", ClampMax = "1000000.0"))
    float MaxRange = 2000.0f;

    /** Задержка между выстрелами в секундах */
    UPROPERTY(EditDefaultsOnly,
        BlueprintReadWrite,
        Category = "Weapon Settings",
        meta = (ClampMin = "0.01", ClampMax = "12.0"))
    float ShotDelay = 0.1f;

    /** Разброс пули в градусах */
    UPROPERTY(EditDefaultsOnly,
        BlueprintReadWrite,
        Category = "Weapon Settings",
        meta = (ClampMin = "0.01", ClampMax = "12.0"))
    float BulletSpread = 1.5f;

    /** Количество урона */
    UPROPERTY(EditDefaultsOnly,
        BlueprintReadWrite,
        Category = "Weapon Settings",
        meta = (ClampMin = "0.0", ClampMax = "1000.0"))
    float DamageAmount = 10.0f;

    /** Множитель урона для headshot */
    UPROPERTY(EditDefaultsOnly,
        BlueprintReadWrite,
        Category = "Weapon Settings",
        meta = (ClampMin = "1.0", ClampMax = "10.0"))
    float HeadshotMultiplier = 10.0f;

    /** Имя кости головы для определения headshot */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings")
    FName HeadBoneName = TEXT("b_head");

    /** Имя кости шеи для определения headshot */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings")
    FName NeckBoneName = TEXT("b_Neck");

    /** Тип урона */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings")
    TSubclassOf<UDamageType> DamageType;

    /** Включить влияние здоровья на разброс выстрелов */
    UPROPERTY(EditDefaultsOnly,
        BlueprintReadWrite,
        Category = "Weapon Settings",
        meta = (ToolTip = "Включить модификацию разброса на основе здоровья"))
    bool bEnableHealthSpreadModifier = false;

    /** Пороговое значение здоровья для начала увеличения разброса (в процентах от 0.0 до 1.0) */
    UPROPERTY(EditDefaultsOnly,
        BlueprintReadWrite,
        Category = "Weapon Settings",
        meta = (EditCondition = "bEnableHealthSpreadModifier",
            ClampMin = "0.0",
            ClampMax = "1.0",
            ToolTip = "Порог здоровья, ниже которого начинает увеличиваться разброс"))
    float HealthSpreadThreshold = 0.3f;

    /** Максимальный множитель разброса при критически низком здоровье */
    UPROPERTY(EditDefaultsOnly,
        BlueprintReadWrite,
        Category = "Weapon Settings",
        meta = (EditCondition = "bEnableHealthSpreadModifier",
            ClampMin = "1.0",
            ClampMax = "10.0",
            ToolTip = "Максимальный множитель разброса при критически низком здоровье"))
    float MaxHealthSpreadMultiplier = 3.0f;

    /**
     * Получает разброс в радианах
     * @return Разброс в радианах
     */
    UFUNCTION(BlueprintPure, Category = "Weapon Configuration")
    float GetSpreadRadians() const { return FMath::DegreesToRadians(BulletSpread); }

    /**
     * Проверяет, является ли попадание headshot'ом
     * @param HitBoneName Имя пораженной кости
     * @return True если это headshot
     */
    UFUNCTION(BlueprintPure, Category = "Weapon Configuration")
    bool IsHeadshot(const FName& HitBoneName) const;

private:
    /**
     * Исправляет значение в заданном диапазоне
     * @param Value Значение для исправления
     * @param MinValue Минимальное значение
     * @param MaxValue Максимальное значение
     * @param ValueName Имя значения для логирования
     */
    void ClampValue(float& Value, float MinValue, float MaxValue, const FString& ValueName) const;
};
