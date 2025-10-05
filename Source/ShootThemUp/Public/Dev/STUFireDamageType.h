// Игра ShootThemUp. Все права защищены.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "STUFireDamageType.generated.h"

/**
 * Тип урона от огня.
 * Используется для обозначения урона, нанесенного огнем.
 */
UCLASS()
class SHOOTTHEMUP_API USTUFireDamageType : public UDamageType
{
    GENERATED_BODY()
};