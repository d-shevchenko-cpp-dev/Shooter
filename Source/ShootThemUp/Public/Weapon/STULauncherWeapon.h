#pragma once

#include "CoreMinimal.h"
#include "Weapon/STUBaseWeapon.h"
#include "STULauncherWeapon.generated.h"

class ASTUProjectile;

UCLASS(BlueprintType, Blueprintable)
class SHOOTTHEMUP_API ASTULauncherWeapon : public ASTUBaseWeapon
{
    GENERATED_BODY()

public:
    ASTULauncherWeapon();

    virtual void StartFire() override;
    virtual void StopFire() override;

protected:
    virtual void MakeShot();

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    TSubclassOf<ASTUProjectile> ProjectileClass;

private:
    void LaunchProjectile(const FVector& TraceStart, const FVector& TraceEnd);
};
