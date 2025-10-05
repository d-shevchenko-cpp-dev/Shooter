// Игра ShootThemUp. Все права защищены.

#include "Weapon/STUWeaponDamageType.h"

USTUWeaponDamageType::USTUWeaponDamageType()
{
    // Установка свойств по умолчанию для урона от оружия
    bCausedByWorld = false;
    bScaleMomentumByMass = true;
    bRadialDamageVelChange = true;
}