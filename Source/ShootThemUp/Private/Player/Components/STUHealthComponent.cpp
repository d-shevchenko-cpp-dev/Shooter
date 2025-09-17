// ShootThemUp Game. All Right Reserved.


#include "Player/Components/STUHealthComponent.h"

// Sets default values for this component's properties
USTUHealthComponent::USTUHealthComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

float USTUHealthComponent::GetHealth() const
{
    return Health;
}

void USTUHealthComponent::BeginPlay()
{
    Super::BeginPlay();

    Health = MaxHealth;
}
