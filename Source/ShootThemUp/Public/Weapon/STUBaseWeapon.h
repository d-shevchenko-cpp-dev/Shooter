// ShootThemUp Game. All Right Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STUBaseWeapon.generated.h"

class USkeletalMeshComponent;
class USTUDamageComponent;

UCLASS()
class SHOOTTHEMUP_API ASTUBaseWeapon : public AActor
{
    GENERATED_BODY()

public:
    ASTUBaseWeapon();

    virtual void Fire();
    virtual void StopFire();

protected:
    virtual void BeginPlay() override;

    void MakeShot();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
    USkeletalMeshComponent* WeaponMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
    USTUDamageComponent* DamageComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings")
    FName MuzzleSocketName { "MuzzleSocket" };

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings", meta = (ClampMin = "100.0", ClampMax = "1000000.0"))
    float MaxRange { 2000.0f };

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings")
    bool bDrawDebugTrace { true };

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings", meta = (EditCondition = "bDrawDebugTrace"))
    float DebugTraceDuration { 3.0f };

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings", meta = (EditCondition = "bDrawDebugTrace"))
    float DebugHitSphereRadius { 10.0f };

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings", meta = (ClampMin = "0.0", ClampMax = "1000.0"))
    float DamageAmount { 10.0f };

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings")
    TSubclassOf<UDamageType> DamageType;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings", meta = (ClampMin = "1.0", ClampMax = "10.0"))
    float HeadshotMultiplier { 2.0f };

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings")
    FName HeadBoneName { "b_head" };

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings")
    FName NeckBoneName { "b_Neck" };

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings", meta = (ClampMin = "0.01", ClampMax = "12.0"))
    float ShotDelay { 0.1f };

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings", meta = (ClampMin = "0.01", ClampMax = "12.0"))
    float BulletSpread { 1.5f };


private:
    FTimerHandle ShotTimerHandle;

private:
    void PerformLineTrace(const FVector& TraceStart, const FVector& TraceEnd, FHitResult& HitResult) const;
    void DrawDebugTrace(const FVector& TraceStart, const FVector& TraceEnd, const FHitResult& HitResult) const;
    bool IsValidForShooting() const;
    void ApplyDamageToTarget(AActor* Target, const FHitResult& HitResult);
};