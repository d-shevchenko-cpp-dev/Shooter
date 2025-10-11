// ShootThemUp Game. All Right Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/STUBaseWeapon.h"
#include "STULauncherWeapon.generated.h"

class ASTUProjectile;

/**
 * Класс гранатомета.
 * Реализует логику запуска снарядов.
 */
UCLASS(BlueprintType, Blueprintable)
class SHOOTTHEMUP_API ASTULauncherWeapon : public ASTUBaseWeapon
{
    GENERATED_BODY()

public:
    ASTULauncherWeapon();

    virtual void Fire() override;

    virtual void StopFire() override;

protected:
    virtual void MakeShot() override;

    /** Класс снаряда для запуска */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    TSubclassOf<ASTUProjectile> ProjectileClass;

private:
    void LaunchProjectile(const FVector& TraceStart, const FVector& TraceEnd);
};
