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

    virtual void StartFire() override;
    virtual void StopFire() override;

protected:
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual void StartAutomaticFire();

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual void StopAutomaticFire();

    virtual void MakeShot();

private:
    void DrawDebugInformation(const FVector& TraceStart, const FVector& TraceEnd, const FHitResult& HitResult) const;

    void ProcessHitResult(const FHitResult& HitResult);

private:
    FTimerHandle ShotTimerHandle;
};
