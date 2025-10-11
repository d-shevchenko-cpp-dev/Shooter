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

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual void Fire() {}

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual void StopFire() {}

    UFUNCTION(BlueprintPure, Category = "Weapon")
    virtual bool CanFire() const;

    UFUNCTION(BlueprintPure, Category = "Weapon")
    const USTUWeaponConfiguration* GetWeaponConfiguration() const { return WeaponConfiguration; }

    UPROPERTY(BlueprintAssignable, Category = "Weapon Events")
    FOnWeaponShot OnWeaponShot;

    UPROPERTY(BlueprintAssignable, Category = "Weapon Events")
    FOnWeaponFireStarted OnWeaponFireStarted;

    UPROPERTY(BlueprintAssignable, Category = "Weapon Events")
    FOnWeaponFireStopped OnWeaponFireStopped;

protected:
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual void MakeShot() {}

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual void StartAutomaticFire();

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual void StopAutomaticFire();

    UFUNCTION(BlueprintPure, Category = "Weapon")
    virtual bool IsValidForShooting() const;

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual bool GetShotTrajectoryPoints(FVector& TraceStart, FVector& TraceEnd) const;

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual void PerformLineTrace(const FVector& TraceStart, const FVector& TraceEnd, FHitResult& HitResult) const;

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual float ApplyDamageToTarget(AActor* Target, const FHitResult& HitResult);

    UFUNCTION(BlueprintPure, Category = "Weapon")
    virtual bool IsHeadshot(const FName& HitBoneName) const;

    UFUNCTION(BlueprintPure, Category = "Weapon")
    virtual FVector GetShootDirection(const FVector& BaseDirection) const;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    USkeletalMeshComponent* WeaponMesh;

    /** Компонент для нанесения урона */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    USTUDamageComponent* DamageComponent;

    /** Конфигурация оружия */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings", meta = (AllowPrivateAccess = "true"))
    USTUWeaponConfiguration* WeaponConfiguration;

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

    bool ValidateComponents() const;
};
