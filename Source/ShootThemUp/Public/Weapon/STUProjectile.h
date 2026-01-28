#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STUProjectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;
class USTUWeaponFXComponent;

/**
 * Класс снаряда для гранатомета.
 */
UCLASS(BlueprintType, Blueprintable)
class SHOOTTHEMUP_API ASTUProjectile : public AActor
{
    GENERATED_BODY()

public:
    ASTUProjectile();

    UFUNCTION(BlueprintCallable, Category = "Projectile")
    void SetShootDirection(const FVector& Direction);

    UFUNCTION(BlueprintPure, Category = "Projectile")
    AController* GetOwnerController() const;

protected:
    virtual void BeginPlay() override;

private:
    UFUNCTION()
    void OnProjectileHit(UPrimitiveComponent* HitComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        FVector NormalImpulse,
        const FHitResult& Hit);

    void ApplyExplosionDamage(const FVector& ExplosionLocation);

    void DrawExplosionDebug(const FVector& ExplosionLocation) const;

protected:
    /** Компонент коллизии снаряда */
    UPROPERTY(VisibleDefaultsOnly, Category = "Projectile")
    USphereComponent* CollisionComponent;

    /** Компонент движения снаряда */
    UPROPERTY(VisibleDefaultsOnly, Category = "Projectile")
    UProjectileMovementComponent* MovementComponent;

    UPROPERTY(VisibleAnywhere, Category = "VFX")
    USTUWeaponFXComponent* WeaponFXComponent;

    /** Радиус урона от взрыва */
    UPROPERTY(EditDefaultsOnly,
        BlueprintReadWrite,
        Category = "Projectile Settings",
        meta = (ClampMin = "10.0", ClampMax = "1000.0"))
    float ExplosionRadius = 200.0f;

    /** Количество урона от взрыва */
    UPROPERTY(EditDefaultsOnly,
        BlueprintReadWrite,
        Category = "Projectile Settings",
        meta = (ClampMin = "1.0", ClampMax = "1000.0"))
    float ExplosionDamage = 50.0f;

    /** Применять полный урон всем целям в радиусе */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile Settings")
    bool bApplyFullDamageToAll = false;

    /** Время жизни снаряда в секундах */
    UPROPERTY(EditDefaultsOnly,
        BlueprintReadWrite,
        Category = "Projectile Settings",
        meta = (ClampMin = "1.0", ClampMax = "60.0"))
    float LifeTime = 5.0f;

private:
    /** Направление полета снаряда */
    FVector ShootDirection;
};
