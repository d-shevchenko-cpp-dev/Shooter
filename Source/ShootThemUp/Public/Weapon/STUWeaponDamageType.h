// Игра ShootThemUp. Все права защищены.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "STUWeaponDamageType.generated.h"

/**
 * Базовый тип урона для оружия в игре ShootThemUp.
 * Предоставляет общие свойства для всех типов урона от оружия.
 */
UCLASS()
class SHOOTTHEMUP_API USTUWeaponDamageType : public UDamageType
{
    GENERATED_BODY()

public:
    USTUWeaponDamageType();
};