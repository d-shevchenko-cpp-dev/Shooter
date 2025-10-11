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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnWeaponShot,
    const FHitResult&,
    HitResult,
    float,
    DamageDealt,
    bool,
    bIsHeadshot);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponFireStarted);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponFireStopped);

USTRUCT(BlueprintType)
struct FAmmoData
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    int32 Bullets;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon", meta = (EditCondition = "!Infinite"))
    int32 Clips;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    bool Infinite;
};

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

    void DecreaseAmmo();

    bool IsAmmoEmpty() const;

    bool IsClipEmpty() const;

    void ChangeClip();

    void LogAmmo();

private:
    /**
     * Получает кэшированное значение разброса в радианах с учетом здоровья.
     * @return Разброс в радианах
     */
    float GetCachedSpreadRadians() const;

    bool ValidateComponents() const;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    USkeletalMeshComponent* WeaponMesh;

    /** Компонент для нанесения урона */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    USTUDamageComponent* DamageComponent;

    /** Конфигурация оружия */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
    USTUWeaponConfiguration* WeaponConfiguration;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
    FAmmoData DefaultAmmo{ 15, 10, false };

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
    FName MuzzleSocketName = TEXT("MuzzleSocket");

private:
    /** Таймер для автоматической стрельбы */
    FTimerHandle ShotTimerHandle;

    FAmmoData CurrentAmmo;

    /** Кэшированное значение разброса в радианах для оптимизации */
    mutable float CachedSpreadRadians = -1.0f;
};
