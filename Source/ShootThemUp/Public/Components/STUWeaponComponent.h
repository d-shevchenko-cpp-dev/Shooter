// Игра ShootThemUp. Все права защищены.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "STUWeaponComponent.generated.h"

class ASTUBaseWeapon;

/**
 * Компонент оружия для управления оружием персонажа.
 * Отвечает за создание и управление экземпляром оружия.
 */

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SHOOTTHEMUP_API USTUWeaponComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    USTUWeaponComponent();

    /**
     * Производит выстрел из текущего оружия.
     */
    void Fire();

protected:
    virtual void BeginPlay() override;

private:
    /**
     * Создает экземпляр оружия и прикрепляет его к персонажу.
     */
    void SpawnWeapon();

protected:
    /** Класс оружия для создания */
    UPROPERTY(EditDefaultsOnly, Category="Weapon")
    TSubclassOf<ASTUBaseWeapon> WeaponClass;

    /** Имя точки прикрепления оружия */
    UPROPERTY(EditDefaultsOnly, Category="Weapon")
    FName WeaponAttachPointName = "WeaponSocket";

private:
    /** Текущее оружие */
    ASTUBaseWeapon* CurrentWeapon;

};