#include "Weapon/STUBaseWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/STUDamageComponent.h"
#include "Components/STUHealthComponent.h"
#include "Components/STUAmmoComponent.h"
#include "Weapon/Configuration/STUWeaponConfiguration.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "Engine/Engine.h"

DEFINE_LOG_CATEGORY_STATIC(LogBaseWeapon, All, All);

ASTUBaseWeapon::ASTUBaseWeapon()
{
    PrimaryActorTick.bCanEverTick = false;

    WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
    RootComponent = WeaponMesh;

    DamageComponent = CreateDefaultSubobject<USTUDamageComponent>(TEXT("DamageComponent"));
    AmmoComponent = CreateDefaultSubobject<USTUAmmoComponent>(TEXT("AmmoComponent"));
    WeaponConfiguration = CreateDefaultSubobject<USTUWeaponConfiguration>(TEXT("WeaponConfiguration"));
}

bool ASTUBaseWeapon::CanFire() const
{
    if (!IsValidForShooting() || !WeaponConfiguration)
    {
        return false;
    }

    return true;
}

void ASTUBaseWeapon::BeginPlay()
{
    Super::BeginPlay();

    if (!ValidateComponents())
    {
        UE_LOG(LogBaseWeapon, Error, TEXT("Weapon components validation failed"));
        return;
    }

    if (AmmoComponent)
    {
        AmmoComponent->Initialize({});
    }
}

bool ASTUBaseWeapon::IsValidForShooting() const
{
    check(GetWorld());
    check(WeaponMesh);
    check(AmmoComponent);
    const ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    check(OwnerCharacter);
    const auto* HealthComponent = OwnerCharacter->FindComponentByClass<USTUHealthComponent>();
    check(HealthComponent);
    const APlayerController* PlayerController = OwnerCharacter->GetController<APlayerController>();
    check(PlayerController);

    if (HealthComponent->IsDead() || AmmoComponent->IsAmmoEmpty())
    {
        return false;
    }

    return true;
}

float ASTUBaseWeapon::ApplyDamageToTarget(AActor* Target, const FHitResult& HitResult)
{
    if (!Target || !DamageComponent || !WeaponConfiguration)
    {
        UE_LOG(LogBaseWeapon, Warning, TEXT("Invalid target, damage component, or weapon configuration"));
        return 0.0f;
    }

    float FinalDamage = WeaponConfiguration->DamageAmount;
    bool bIsHeadshot = IsHeadshot(HitResult.BoneName);

    // Применение множителя headshot
    if (bIsHeadshot)
    {
        FinalDamage *= WeaponConfiguration->HeadshotMultiplier;
        UE_LOG(LogBaseWeapon, Log, TEXT("Headshot! Damage multiplied by %f"), WeaponConfiguration->HeadshotMultiplier);
    }

    // Использование компонента урона для нанесения урона
    const bool bDamageDealt = DamageComponent->DealDamage(
        Target, FinalDamage, WeaponConfiguration->DamageType, HitResult.ImpactPoint, HitResult.BoneName, bIsHeadshot);

    if (bDamageDealt)
    {
        UE_LOG(LogBaseWeapon,
            Log,
            TEXT("Successfully dealt %f damage to %s%s"),
            FinalDamage,
            *Target->GetName(),
            bIsHeadshot ? TEXT(" (HEADSHOT)") : TEXT(""));
        return FinalDamage;
    }

    return 0.0f;
}

bool ASTUBaseWeapon::IsHeadshot(const FName& HitBoneName) const
{
    if (!WeaponConfiguration)
    {
        return false;
    }

    return WeaponConfiguration->IsHeadshot(HitBoneName);
}

bool ASTUBaseWeapon::ValidateComponents() const
{
    check(WeaponMesh);
    check(DamageComponent);
    check(AmmoComponent);
    check(WeaponConfiguration);

    return true;
}
