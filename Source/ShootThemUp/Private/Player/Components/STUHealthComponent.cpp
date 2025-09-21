// ShootThemUp Game. All Right Reserved.


#include "Player/Components/STUHealthComponent.h"
#include "GameFramework/Actor.h"

DEFINE_LOG_CATEGORY_STATIC(LogHealtComponent, All, All);

// Sets default values for this component's properties
USTUHealthComponent::USTUHealthComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

float USTUHealthComponent::GetHealth() const
{
    return Health;
}

bool USTUHealthComponent::IsDead() const
{
    return Health <= 0.0f;
}

void USTUHealthComponent::BeginPlay()
{
    Super::BeginPlay();

    Health = MaxHealth;

    OnHealthChanged.Broadcast(Health);

    if (auto* owner = GetOwner())
    {
        owner->OnTakeAnyDamage.AddDynamic(this, &USTUHealthComponent::OnTakeAnyDamageHandle);
    }
}

void USTUHealthComponent::OnTakeAnyDamageHandle(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
    class AController* InstigatedBy, AActor* DamageCauser)
{
    if (Damage <= 0.0f || IsDead())
    {
        return;
    }
    
    Health = FMath::Clamp(Health - Damage, 0.0f, MaxHealth);

    OnHealthChanged.Broadcast(Health);
    
    if (IsDead())
    {
        OnDeath.Broadcast();
    }
}