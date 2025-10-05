// ShootThemUp Game. All Right Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Components/SkeletalMeshComponent.h"
#include "Weapon/Configuration/STUWeaponConfiguration.h"
#include "STUBaseWeapon.generated.h"

class USkeletalMeshComponent;
class USTUDamageComponent;
class UDamageType;
struct FHitResult;

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
 * Базовый класс для всех видов оружия в игре.
 */
UCLASS(BlueprintType, Blueprintable)
class SHOOTTHEMUP_API ASTUBaseWeapon : public AActor
{
    GENERATED_BODY()

public:
    ASTUBaseWeapon();

    /**
     * Начинает стрельбу из оружия.
     * Должен быть переопределен в дочерних классах для конкретной логики стрельбы.
     */
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual void Fire() {}

    /**
     * Останавливает стрельбу из оружия.
     * Должен быть переопределен в дочерних классах для конкретной логики остановки стрельбы.
     */
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual void StopFire() {}

    /**
     * Проверяет, может ли оружие стрелять в данный момент.
     * @return True если оружие может стрелять
     */
    UFUNCTION(BlueprintPure, Category = "Weapon")
    virtual bool CanFire() const;

    /**
     * Получает текущую конфигурацию оружия.
     * @return Конфигурация оружия
     */
    UFUNCTION(BlueprintPure, Category = "Weapon")
    const USTUWeaponConfiguration* GetWeaponConfiguration() const { return WeaponConfiguration; }

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
     * Должен быть переопределен в дочерних классах для конкретной логики выстрела.
     */
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual void MakeShot() {}

    /**
     * Выполняет автоматическую стрельбу с заданным интервалом.
     * Может быть переопределен в дочерних классах для кастомной логики автоматической стрельбы.
     */
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual void StartAutomaticFire();

    /**
     * Останавливает автоматическую стрельбу.
     * Может быть переопределен в дочерних классах для кастомной логики остановки.
     */
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual void StopAutomaticFire();

    /**
     * Проверяет валидность оружия для стрельбы.
     * @return True если оружие готово к стрельбе
     */
    UFUNCTION(BlueprintPure, Category = "Weapon")
    virtual bool IsValidForShooting() const;

    /**
     * Получает начальную и конечную точки траектории выстрела.
     * @param TraceStart Начальная точка трейсинга (выход)
     * @param TraceEnd Конечная точка трейсинга (выход)
     * @return True если точки успешно получены, False в случае ошибки
     */
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual bool GetShotTrajectoryPoints(FVector& TraceStart, FVector& TraceEnd) const;

    /**
     * Выполняет линейный трейсинг от начальной до конечной точки.
     * @param TraceStart Начальная точка трейсинга
     * @param TraceEnd Конечная точка трейсинга
     * @param HitResult Результат трейсинга
     */
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual void PerformLineTrace(const FVector& TraceStart, const FVector& TraceEnd, FHitResult& HitResult) const;

    /**
     * Применяет урон к цели на основе результата попадания.
     * @param Target Целевой актор
     * @param HitResult Результат попадания
     * @return Количество нанесенного урона
     */
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual float ApplyDamageToTarget(AActor* Target, const FHitResult& HitResult);

    /**
     * Определяет, является ли попадание headshot'ом.
     * @param HitBoneName Имя пораженной кости
     * @return True если это headshot
     */
    UFUNCTION(BlueprintPure, Category = "Weapon")
    virtual bool IsHeadshot(const FName& HitBoneName) const;

    /**
     * Получает направление стрельбы с учетом разброса.
     * @param BaseDirection Базовое направление стрельбы
     * @return Направление стрельбы с разбросом
     */
    UFUNCTION(BlueprintPure, Category = "Weapon")
    virtual FVector GetShootDirection(const FVector& BaseDirection) const;

protected:
    /** Компонент меша оружия */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    USkeletalMeshComponent* WeaponMesh;

    /** Компонент для нанесения урона */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    USTUDamageComponent* DamageComponent;

    /** Конфигурация оружия */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings", meta = (AllowPrivateAccess = "true"))
    USTUWeaponConfiguration* WeaponConfiguration;

    /** Имя сокета дула оружия */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings", meta = (AllowPrivateAccess = "true"))
    FName MuzzleSocketName = TEXT("MuzzleSocket");

private:
    /** Таймер для автоматической стрельбы */
    FTimerHandle ShotTimerHandle;

    /** Кэшированное значение разброса в радианах для оптимизации */
    mutable float CachedSpreadRadians = -1.0f;

    /**
     * Получает кэшированное значение разброса в радианах с учетом здоровья.
     * @return Разброс в радианах
     */
    float GetCachedSpreadRadians() const;

    /**
     * Инициализирует сервисы оружия.
     */
    void InitializeServices();

    /**
     * Валидирует компоненты оружия.
     * @return True если все компоненты валидны
     */
    bool ValidateComponents() const;
};
