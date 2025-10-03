// ShootThemUp Game. All Right Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STUBaseWeapon.generated.h"

class USkeletalMeshComponent;

UCLASS()
class SHOOTTHEMUP_API ASTUBaseWeapon : public AActor
{
    GENERATED_BODY()

public:
    ASTUBaseWeapon();

    virtual void Fire();

protected:
    virtual void BeginPlay() override;

    void MakeShot();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
    USkeletalMeshComponent* WeaponMesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings")
    FName MuzzleSocketName { "MuzzleSocket" };

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings", meta = (ClampMin = "100.0", ClampMax = "10000.0"))
    float MaxRange { 2000.0f };

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings")
    bool bDrawDebugTrace { false };

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings", meta = (EditCondition = "bDrawDebugTrace"))
    float DebugTraceDuration { 3.0f };

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Settings", meta = (EditCondition = "bDrawDebugTrace"))
    float DebugHitSphereRadius { 10.0f };

private:
    void PerformLineTrace(const FVector& TraceStart, const FVector& TraceEnd, FHitResult& HitResult) const;
    void DrawDebugTrace(const FVector& TraceStart, const FVector& TraceEnd, const FHitResult& HitResult) const;
    bool IsValidForShooting() const;
};