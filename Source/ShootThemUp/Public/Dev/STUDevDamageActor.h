// Игра ShootThemUp. Все права защищены.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STUDevDamageActor.generated.h"

/**
 * Актор для разработки, который наносит радиальный урон.
 * Используется для тестирования системы урона в редакторе.
 */
UCLASS()
class SHOOTTHEMUP_API ASTUDevDamageActor : public AActor
{
    GENERATED_BODY()

public:
    ASTUDevDamageActor();

    virtual void Tick(float DeltaTime) override;

protected:
    virtual void BeginPlay() override;

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    USceneComponent* SceneComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Radius{300.0f};

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FColor SphereColor{FColor::Red};

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Damage{10.0f};

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool DoFullDamage{false};

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<UDamageType> DamageType;
};