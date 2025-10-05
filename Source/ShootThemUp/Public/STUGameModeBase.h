// Игра ShootThemUp. Все права защищены.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "STUGameModeBase.generated.h"

/**
 * Базовый игровой режим для ShootThemUp.
 * Настраивает классы по умолчанию для персонажа, контроллера и HUD.
 */
UCLASS()
class SHOOTTHEMUP_API ASTUGameModeBase : public AGameModeBase
{
    GENERATED_BODY()

public:
    ASTUGameModeBase();
};
