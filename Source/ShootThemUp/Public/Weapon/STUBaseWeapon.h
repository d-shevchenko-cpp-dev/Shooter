#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Components/SkeletalMeshComponent.h"
#include "Weapon/Configuration/STUWeaponConfiguration.h"
#include "Weapon/STUWeaponTypes.h"
#include "STUBaseWeapon.generated.h"

class USkeletalMeshComponent;
class USTUDamageComponent;
class USTUAmmoComponent;
struct FHitResult;
struct FAmmoData;

UCLASS(BlueprintType, Blueprintable)
class SHOOTTHEMUP_API ASTUBaseWeapon : public AActor
{
    GENERATED_BODY()

public:
    ASTUBaseWeapon();

    virtual void StartFire() {}

    virtual void StopFire() {}

    virtual bool CanFire() const;

    UFUNCTION(BlueprintPure, Category = "Weapon")
    const USTUWeaponConfiguration* GetWeaponConfiguration() const { return WeaponConfiguration; }

    UFUNCTION(BlueprintPure, Category = "Weapon")
    USTUAmmoComponent* GetAmmoComponent() const { return AmmoComponent; }

    UFUNCTION(BlueprintPure, Category = "Weapon")
    USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }

    FWeaponUIData GetUIData() const { return UIData; }

protected:
    virtual void BeginPlay() override;

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

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
    USTUWeaponConfiguration* WeaponConfiguration;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
    FName MuzzleSocketName = TEXT("MuzzleSocket");

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    FWeaponUIData UIData;

private:
    bool ValidateComponents() const;
};
