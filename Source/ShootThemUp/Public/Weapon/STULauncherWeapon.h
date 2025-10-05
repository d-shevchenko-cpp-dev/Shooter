// ShootThemUp Game. All Right Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/STUBaseWeapon.h"
#include "STULauncherWeapon.generated.h"

/**
 * Класс гранатомета/ракетницы.
 * Базовый класс для оружия, стреляющего снарядами (гранаты, ракеты).
 * В будущем можно расширить для реализации логики запуска снарядов.
 */
UCLASS(BlueprintType, Blueprintable)
class SHOOTTHEMUP_API ASTULauncherWeapon : public ASTUBaseWeapon
{
    GENERATED_BODY()

public:
    ASTULauncherWeapon();

    /**
     * Начинает стрельбу из гранатомета.
     * В будущем здесь будет логика запуска снарядов.
     */
    virtual void Fire() override;

    /**
     * Останавливает стрельбу из гранатомета.
     * В будущем здесь будет логика остановки запуска снарядов.
     */
    virtual void StopFire() override;

protected:
    /**
     * Выполняет один выстрел из гранатомета.
     * В будущем здесь будет логика создания и запуска снаряда.
     */
    virtual void MakeShot() override;

    // TODO: В будущем добавить:
    // - ProjectileClass для типа снаряда
    // - LaunchForce для силы запуска
    // - ProjectileSpawnSocket для сокета спавна снаряда
    // - bUseGravity для использования гравитации снаряда
};
