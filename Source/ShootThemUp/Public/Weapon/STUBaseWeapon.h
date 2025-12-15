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
class USTUAmmoComponent;
struct FHitResult;
struct FAmmoData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponFireStarted);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponFireStopped);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnWeaponShot,
    const FHitResult&,
    HitResult,
    float,
    DamageDealt,
    bool,
    bIsHeadshot);

UCLASS(BlueprintType, Blueprintable)
class SHOOTTHEMUP_API ASTUBaseWeapon : public AActor
{
    GENERATED_BODY()

public:
    ASTUBaseWeapon();

    virtual void StartFire() {}

    virtual void StopFire() {}

    virtual bool CanFire() const;

    /** Получает конфигурацию оружия */
    UFUNCTION(BlueprintPure, Category = "Weapon")
    const USTUWeaponConfiguration* GetWeaponConfiguration() const { return WeaponConfiguration; }

    UFUNCTION(BlueprintPure, Category = "Weapon")
    USTUAmmoComponent* GetAmmoComponent() const { return AmmoComponent; }

    UFUNCTION(BlueprintPure, Category = "Weapon")
    USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }

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
    virtual float ApplyDamageToTarget(AActor* Target, const FHitResult& HitResult);

    UFUNCTION(BlueprintPure, Category = "Weapon")
    virtual bool IsHeadshot(const FName& HitBoneName) const;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    USkeletalMeshComponent* WeaponMesh;

    /** Компонент для нанесения урона */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    USTUDamageComponent* DamageComponent;

    /** Компонент управления патронами */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USTUAmmoComponent* AmmoComponent;

    /** Конфигурация оружия */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
    USTUWeaponConfiguration* WeaponConfiguration;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
    FName MuzzleSocketName = TEXT("MuzzleSocket");

private:
    bool ValidateComponents() const;

    FTimerHandle ShotTimerHandle;
};
