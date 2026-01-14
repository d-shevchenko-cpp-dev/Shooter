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

protected:
    virtual void MakeShot();

    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    TSubclassOf<ASTUProjectile> ProjectileClass;

private:
    void LaunchProjectile(const FVector& TraceStart, const FVector& TraceEnd);
};
