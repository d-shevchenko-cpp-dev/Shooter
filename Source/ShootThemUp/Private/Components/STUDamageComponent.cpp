// Игра ShootThemUp. Все права защищены.

#include "Components/STUDamageComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogDamageComponent, All, All);

USTUDamageComponent::USTUDamageComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void USTUDamageComponent::BeginPlay()
{
    Super::BeginPlay();
}

bool USTUDamageComponent::DealDamage(AActor* Target, float DamageAmount, TSubclassOf<UDamageType> DamageType,
                                   const FVector& HitLocation, const FName& HitBoneName, bool bIsHeadshot)
{
    if (!Target || DamageAmount <= 0.0f)
    {
        return false;
    }

    return ApplyDamageInternal(Target, DamageAmount, DamageType, HitLocation, HitBoneName, bIsHeadshot);
}

bool USTUDamageComponent::ApplyDamageInternal(AActor* Target, float DamageAmount, TSubclassOf<UDamageType> DamageType,
                                            const FVector& HitLocation, const FName& HitBoneName, bool bIsHeadshot)
{
    if (!Target || !GetWorld())
    {
        return false;
    }

    // Получение контроллера инициатора
    AController* InstigatorController = GetInstigatorController();

    // Создание результата попадания для применения урона
    FHitResult HitResult;
    HitResult.Actor = Target;
    HitResult.Location = HitLocation;
    HitResult.BoneName = HitBoneName;
    HitResult.ImpactNormal = FVector::UpVector; // Default impact normal

    // Применение точечного урона с использованием системы урона Unreal
    UGameplayStatics::ApplyPointDamage(
        Target,                          // Target actor
        DamageAmount,                   // Damage amount
        HitResult.ImpactNormal,          // Damage direction
        HitResult,                      // Hit result
        InstigatorController,            // Instigator controller
        GetOwner(),                     // Damage causer
        DamageType                      // Damage type
    );

    UE_LOG(LogDamageComponent, Log, TEXT("Dealt %f damage to %s%s"), 
        DamageAmount,
        Target ? *Target->GetName() : TEXT("Unknown"),
        bIsHeadshot ? TEXT(" (HEADSHOT)") : TEXT(""));

    return true;
}

AController* USTUDamageComponent::GetInstigatorController() const
{
    if (const ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner()))
    {
        return OwnerCharacter->GetController();
    }
    
    return nullptr;
}