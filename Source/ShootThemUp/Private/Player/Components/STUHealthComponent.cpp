// ShootThemUp Game. All Right Reserved.


#include "Player/Components/STUHealthComponent.h"

#include "STUFireDamageType.h"
#include "STUIceDamageType.h"
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

void USTUHealthComponent::BeginPlay()
{
    Super::BeginPlay();

    Health = MaxHealth;

    if (auto* owner = GetOwner())
    {
        owner->OnTakeAnyDamage.AddDynamic(this, &USTUHealthComponent::OnTakeAnyDamageHandle);
    }
}

void USTUHealthComponent::OnTakeAnyDamageHandle(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
    class AController* InstigatedBy, AActor* DamageCauser)
{
    Health -= Damage;
    UE_LOG(LogHealtComponent, Display, TEXT("Damage = %f"), Damage);

    if (DamageType)
    {
        if (DamageType->IsA<USTUFireDamageType>())
        {
            UE_LOG(LogHealtComponent, Display, TEXT("Hot!"));
        }
        else if (DamageType->IsA<USTUIceDamageType>())
        {
            UE_LOG(LogHealtComponent, Display, TEXT("Cold!"));
        }
    }
}