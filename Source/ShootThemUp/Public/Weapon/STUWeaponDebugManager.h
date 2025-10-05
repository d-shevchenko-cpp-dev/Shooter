// Игра ShootThemUp. Все права защищены.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "STUWeaponDebugManager.generated.h"

/**
 * Категории отладочной информации для оружия
 */
UENUM(BlueprintType)
enum class ESTUWeaponDebugCategory : uint8
{
    None = 0,
    Trace = 1 << 0,  // Линии трейсинга
    Hit = 1 << 1,    // Точки попадания
    Camera = 1 << 2, // Направление камеры
    Spread = 1 << 3, // Разброс пуль
    All = Trace | Hit | Camera | Spread
};

/**
 * Структура настроек отладки для оружия
 */
USTRUCT(BlueprintType)
struct SHOOTTHEMUP_API FSTUWeaponDebugSettings
{
    GENERATED_BODY()

    /** Включить отрисовку отладочной информации */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Debug Settings")
    bool bEnableDebug = false;

    /** Категории отладочной информации для отображения */
    UPROPERTY(EditDefaultsOnly,
        BlueprintReadWrite,
        Category = "Debug Settings",
        meta = (EditCondition = "bEnableDebug"))
    int32 DebugCategories = static_cast<int32>(ESTUWeaponDebugCategory::All);

    /** Длительность отображения отладочной информации */
    UPROPERTY(EditDefaultsOnly,
        BlueprintReadWrite,
        Category = "Debug Settings",
        meta = (EditCondition = "bEnableDebug", ClampMin = "0.1", ClampMax = "30.0"))
    float DebugDuration = 3.0f;

    /** Толщина линий трейсинга */
    UPROPERTY(EditDefaultsOnly,
        BlueprintReadWrite,
        Category = "Debug Settings",
        meta = (EditCondition = "bEnableDebug", ClampMin = "1.0", ClampMax = "10.0"))
    float TraceLineThickness = 3.0f;

    /** Радиус сферы в точке попадания */
    UPROPERTY(EditDefaultsOnly,
        BlueprintReadWrite,
        Category = "Debug Settings",
        meta = (EditCondition = "bEnableDebug", ClampMin = "1.0", ClampMax = "50.0"))
    float HitSphereRadius = 10.0f;

    /** Радиус сферы для отображения разброса */
    UPROPERTY(EditDefaultsOnly,
        BlueprintReadWrite,
        Category = "Debug Settings",
        meta = (EditCondition = "bEnableDebug", ClampMin = "1.0", ClampMax = "100.0"))
    float SpreadSphereRadius = 5.0f;

    /** Цвет линий трейсинга */
    UPROPERTY(EditDefaultsOnly,
        BlueprintReadWrite,
        Category = "Debug Settings",
        meta = (EditCondition = "bEnableDebug"))
    FColor TraceLineColor = FColor::Red;

    /** Цвет точек попадания */
    UPROPERTY(EditDefaultsOnly,
        BlueprintReadWrite,
        Category = "Debug Settings",
        meta = (EditCondition = "bEnableDebug"))
    FColor HitSphereColor = FColor::Red;

    /** Цвет направления камеры */
    UPROPERTY(EditDefaultsOnly,
        BlueprintReadWrite,
        Category = "Debug Settings",
        meta = (EditCondition = "bEnableDebug"))
    FColor CameraLineColor = FColor::Blue;

    /** Цвет разброса пуль */
    UPROPERTY(EditDefaultsOnly,
        BlueprintReadWrite,
        Category = "Debug Settings",
        meta = (EditCondition = "bEnableDebug"))
    FColor SpreadColor = FColor::Yellow;

    FSTUWeaponDebugSettings()
    {
        bEnableDebug = false;
        DebugCategories = static_cast<int32>(ESTUWeaponDebugCategory::All);
        DebugDuration = 3.0f;
        TraceLineThickness = 3.0f;
        HitSphereRadius = 10.0f;
        SpreadSphereRadius = 5.0f;
        TraceLineColor = FColor::Red;
        HitSphereColor = FColor::Red;
        CameraLineColor = FColor::Blue;
        SpreadColor = FColor::Yellow;
    }
};

/**
 * Менеджер отладочной информации для оружия.
 * Предоставляет централизованное управление отрисовкой отладочной информации.
 */
UCLASS(BlueprintType, Blueprintable)
class SHOOTTHEMUP_API USTUWeaponDebugManager : public UActorComponent
{
    GENERATED_BODY()

public:
    USTUWeaponDebugManager();

    /**
     * Инициализирует менеджер отладки с настройками
     * @param Settings Настройки отладки
     */
    UFUNCTION(BlueprintCallable, Category = "Weapon Debug")
    void Initialize(const FSTUWeaponDebugSettings& Settings);

    /**
     * Проверяет, включена ли отладка для указанной категории
     * @param Category Категория отладки
     * @return True если отладка включена для данной категории
     */
    UFUNCTION(BlueprintPure, Category = "Weapon Debug")
    bool IsDebugEnabledForCategory(ESTUWeaponDebugCategory Category) const;

    /**
     * Включает или выключает отладку для указанной категории
     * @param Category Категория отладки
     * @param bEnabled Включить или выключить
     */
    UFUNCTION(BlueprintCallable, Category = "Weapon Debug")
    void SetDebugEnabledForCategory(ESTUWeaponDebugCategory Category, bool bEnabled);

    /**
     * Включает или выключает всю отладку
     * @param bEnabled Включить или выключить
     */
    UFUNCTION(BlueprintCallable, Category = "Weapon Debug")
    void SetDebugEnabled(bool bEnabled);

    /**
     * Получает текущие настройки отладки
     * @return Настройки отладки
     */
    UFUNCTION(BlueprintPure, Category = "Weapon Debug")
    const FSTUWeaponDebugSettings& GetDebugSettings() const { return DebugSettings; }

    /**
     * Обновляет настройки отладки
     * @param Settings Новые настройки
     */
    UFUNCTION(BlueprintCallable, Category = "Weapon Debug")
    void UpdateDebugSettings(const FSTUWeaponDebugSettings& Settings);

    /**
     * Отрисовывает линию трейсинга
     * @param World Мир для отрисовки
     * @param Start Начальная точка
     * @param End Конечная точка
     */
    UFUNCTION(BlueprintCallable, Category = "Weapon Debug")
    void DrawTraceLine(UWorld* World, const FVector& Start, const FVector& End) const;

    /**
     * Отрисовывает сферу в точке попадания
     * @param World Мир для отрисовки
     * @param Location Позиция сферы
     */
    UFUNCTION(BlueprintCallable, Category = "Weapon Debug")
    void DrawHitSphere(UWorld* World, const FVector& Location) const;

    /**
     * Отрисовывает линию направления камеры
     * @param World Мир для отрисовки
     * @param Start Начальная точка
     * @param End Конечная точка
     */
    UFUNCTION(BlueprintCallable, Category = "Weapon Debug")
    void DrawCameraLine(UWorld* World, const FVector& Start, const FVector& End) const;

    /**
     * Отрисовывает сферу разброса
     * @param World Мир для отрисовки
     * @param Location Позиция сферы
     */
    UFUNCTION(BlueprintCallable, Category = "Weapon Debug")
    void DrawSpreadSphere(UWorld* World, const FVector& Location) const;

private:
    /** Настройки отладки */
    UPROPERTY()
    FSTUWeaponDebugSettings DebugSettings;
};
