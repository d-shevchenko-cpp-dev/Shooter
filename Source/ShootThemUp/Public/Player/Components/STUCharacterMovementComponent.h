// Игра ShootThemUp. Все права защищены.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "STUCharacterMovementComponent.generated.h"

/**
 * Компонент движения персонажа с поддержкой бега.
 * Расширяет стандартный компонент движения для добавления модификатора скорости бега.
 */
UCLASS()
class SHOOTTHEMUP_API USTUCharacterMovementComponent : public UCharacterMovementComponent
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Movement", meta=(ClampMin = 1.5f, ClampMax = 10.f))
    float RunModifier{2.f};

    virtual float GetMaxSpeed() const override;

};