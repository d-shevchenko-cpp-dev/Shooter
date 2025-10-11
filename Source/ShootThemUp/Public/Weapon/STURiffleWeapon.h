// ShootThemUp Game. All Right Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/STUBaseWeapon.h"
#include "STURiffleWeapon.generated.h"

UCLASS(BlueprintType, Blueprintable)
class SHOOTTHEMUP_API ASTURiffleWeapon : public ASTUBaseWeapon
{
    GENERATED_BODY()

public:
    ASTURiffleWeapon();

    virtual void Fire() override;

    virtual void StopFire() override;

protected:
    virtual void MakeShot() override;

private:
    void DrawDebugInformation(const FVector& TraceStart, const FVector& TraceEnd, const FHitResult& HitResult) const;

    void ProcessHitResult(const FHitResult& HitResult);
};
