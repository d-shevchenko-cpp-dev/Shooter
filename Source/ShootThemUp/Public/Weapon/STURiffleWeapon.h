// ShootThemUp Game. All Right Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/STUBaseWeapon.h"
#include "STURiffleWeapon.generated.h"

/**
 * Класс автоматической винтовки.
 * Реализует логику автоматической стрельбы с трейсингом и нанесением урона.
 */
UCLASS(BlueprintType, Blueprintable)
class SHOOTTHEMUP_API ASTURiffleWeapon : public ASTUBaseWeapon
{
    GENERATED_BODY()

public:
    ASTURiffleWeapon();

    /**
     * Начинает автоматическую стрельбу из винтовки.
     */
    virtual void Fire() override;

    /**
     * Останавливает автоматическую стрельбу из винтовки.
     */
    virtual void StopFire() override;

protected:
    /**
     * Выполняет один выстрел из винтовки.
     * Реализует логику трейсинга и нанесения урона.
     */
    virtual void MakeShot() override;
};
