// ShootThemUp Game. All Right Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Components/SkeletalMeshComponent.h"
#include "STUBaseWeapon.generated.h"

class USkeletalMeshComponent;
class USTUDamageComponent;
class UDamageType;

/**
 * Делегат для события выстрела из оружия.
 * @param HitResult Результат попадания (может быть пустым если промах)
 * @param DamageDealt Количество нанесенного урона (0 если промах)
 * @param bIsHeadshot Является ли попадание headshot'ом
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnWeaponShot,
    const FHitResult&,
    HitResult,
    float,
    DamageDealt,
    bool,
    bIsHeadshot);

/**
 * Делегат для события начала стрельбы.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponFireStarted);

/**
 * Делегат для события остановки стрельбы.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponFireStopped);

/**
 * Структура данных для настроек оружия.
 * Централизует все параметры оружия в одном месте.
 */
USTRUCT(BlueprintType)
struct SHOOTTHEMUP_API FSTUWeaponData
{
    GENERATED_BODY()

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
    float HeadshotMultiplier = 2.0f;

    /** Имя кости головы для определения headshot */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings")
    FName HeadBoneName = TEXT("b_head");

    /** Имя кости шеи для определения headshot */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings")
    FName NeckBoneName = TEXT("b_Neck");

    /** Тип урона */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings")
    TSubclassOf<UDamageType> DamageType;

    FSTUWeaponData()
    {
        MaxRange = 2000.0f;
        ShotDelay = 0.1f;
        BulletSpread = 1.5f;
        DamageAmount = 10.0f;
        HeadshotMultiplier = 2.0f;
        HeadBoneName = TEXT("b_head");
        NeckBoneName = TEXT("b_Neck");
        DamageType = nullptr;
    }
};

/**
 * Структура данных для настроек отладки оружия.
 */
USTRUCT(BlueprintType)
struct SHOOTTHEMUP_API FSTUWeaponDebugData
{
    GENERATED_BODY()

    /** Включить отрисовку отладочной информации */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Debug Settings")
    bool bDrawDebugTrace = true;

    /** Длительность отображения отладочной информации */
    UPROPERTY(EditDefaultsOnly,
        BlueprintReadWrite,
        Category = "Debug Settings",
        meta = (EditCondition = "bDrawDebugTrace"))
    float DebugTraceDuration = 3.0f;

    /** Радиус сферы в точке попадания */
    UPROPERTY(EditDefaultsOnly,
        BlueprintReadWrite,
        Category = "Debug Settings",
        meta = (EditCondition = "bDrawDebugTrace"))
    float DebugHitSphereRadius = 10.0f;

    FSTUWeaponDebugData()
    {
        bDrawDebugTrace = true;
        DebugTraceDuration = 3.0f;
        DebugHitSphereRadius = 10.0f;
    }
};

/**
 * Базовый класс для всех видов оружия в игре.
 * Предоставляет основную функциональность стрельбы, трейсинга и нанесения урона.
 */
UCLASS(BlueprintType, Blueprintable)
class SHOOTTHEMUP_API ASTUBaseWeapon : public AActor
{
    GENERATED_BODY()

public:
    /** Константы для лучшей читаемости кода */
    static constexpr float MIN_SHOT_DELAY = 0.01f;
    static constexpr float MAX_SHOT_DELAY = 12.0f;
    static constexpr float MIN_DAMAGE = 0.0f;
    static constexpr float MAX_DAMAGE = 1000.0f;
    static constexpr float MIN_HEADSHOT_MULTIPLIER = 1.0f;
    static constexpr float MAX_HEADSHOT_MULTIPLIER = 10.0f;

    ASTUBaseWeapon();

    /**
     * Начинает стрельбу из оружия.
     * Устанавливает таймер для автоматической стрельбы.
     */
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual void Fire();

    /**
     * Останавливает стрельбу из оружия.
     * Очищает таймер автоматической стрельбы.
     */
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual void StopFire();

    /**
     * Проверяет, может ли оружие стрелять в данный момент.
     * @return True если оружие может стрелять
     */
    UFUNCTION(BlueprintPure, Category = "Weapon")
    virtual bool CanFire() const;

    /**
     * Получает текущие данные оружия.
     * @return Структура с данными оружия
     */
    UFUNCTION(BlueprintPure, Category = "Weapon")
    const FSTUWeaponData& GetWeaponData() const { return WeaponData; }

    /**
     * Получает данные отладки оружия.
     * @return Структура с данными отладки
     */
    UFUNCTION(BlueprintPure, Category = "Weapon")
    const FSTUWeaponDebugData& GetDebugData() const { return DebugData; }

    /** Событие выстрела из оружия */
    UPROPERTY(BlueprintAssignable, Category = "Weapon Events")
    FOnWeaponShot OnWeaponShot;

    /** Событие начала стрельбы */
    UPROPERTY(BlueprintAssignable, Category = "Weapon Events")
    FOnWeaponFireStarted OnWeaponFireStarted;

    /** Событие остановки стрельбы */
    UPROPERTY(BlueprintAssignable, Category = "Weapon Events")
    FOnWeaponFireStopped OnWeaponFireStopped;

protected:
    virtual void BeginPlay() override;

    /**
     * Выполняет один выстрел из оружия.
     * Основная логика стрельбы с трейсингом и нанесением урона.
     */
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual void MakeShot();

    /**
     * Выполняет линейный трейсинг от начальной до конечной точки.
     * @param TraceStart Начальная точка трейсинга
     * @param TraceEnd Конечная точка трейсинга
     * @param HitResult Результат трейсинга
     */
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual void PerformLineTrace(const FVector& TraceStart, const FVector& TraceEnd, FHitResult& HitResult) const;

    /**
     * Отрисовывает отладочную информацию о трейсинге.
     * @param TraceStart Начальная точка трейсинга
     * @param TraceEnd Конечная точка трейсинга
     * @param HitResult Результат трейсинга
     */
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual void DrawDebugTrace(const FVector& TraceStart, const FVector& TraceEnd, const FHitResult& HitResult) const;

    /**
     * Применяет урон к цели на основе результата попадания.
     * @param Target Целевой актор
     * @param HitResult Результат попадания
     * @return Количество нанесенного урона
     */
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual float ApplyDamageToTarget(AActor* Target, const FHitResult& HitResult);

    /**
     * Проверяет валидность оружия для стрельбы.
     * @return True если оружие готово к стрельбе
     */
    UFUNCTION(BlueprintPure, Category = "Weapon")
    virtual bool IsValidForShooting() const;

    /**
     * Получает направление стрельбы с учетом разброса.
     * @param BaseDirection Базовое направление стрельбы
     * @return Направление стрельбы с разбросом
     */
    UFUNCTION(BlueprintPure, Category = "Weapon")
    virtual FVector GetShootDirection(const FVector& BaseDirection) const;

    /**
     * Определяет, является ли попадание headshot'ом.
     * @param HitBoneName Имя пораженной кости
     * @return True если это headshot
     */
    UFUNCTION(BlueprintPure, Category = "Weapon")
    virtual bool IsHeadshot(const FName& HitBoneName) const;

protected:
    /** Компонент меша оружия */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    USkeletalMeshComponent* WeaponMesh;

    /** Компонент для нанесения урона */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    USTUDamageComponent* DamageComponent;

    /** Имя сокета дула оружия */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings", meta = (AllowPrivateAccess = "true"))
    FName MuzzleSocketName = TEXT("MuzzleSocket");

    /** Данные оружия */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings", meta = (AllowPrivateAccess = "true"))
    FSTUWeaponData WeaponData;

    /** Данные отладки оружия */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Debug Settings", meta = (AllowPrivateAccess = "true"))
    FSTUWeaponDebugData DebugData;

private:
    /** Таймер для автоматической стрельбы */
    FTimerHandle ShotTimerHandle;

    /** Кэшированное значение разброса в радианах для оптимизации */
    mutable float CachedSpreadRadians = -1.0f;

    /**
     * Получает кэшированное значение разброса в радианах.
     * @return Разброс в радианах
     */
    float GetCachedSpreadRadians() const;
};