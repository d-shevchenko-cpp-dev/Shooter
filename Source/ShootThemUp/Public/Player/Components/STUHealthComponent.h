// ShootThemUp Game. All Right Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "STUHealthComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SHOOTTHEMUP_API USTUHealthComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    USTUHealthComponent();

    float GetHealth() const;

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

private:
    UFUNCTION()
    void OnTakeAnyDamageHandle(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser );


protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health", meta=(ClampMin = 0.f, ClampMax = 1000.f))
    float MaxHealth {100.f};

private:
    float Health {0.f};
};