// Игра ShootThemUp. Все права защищены.

#include "Components/STUWeaponComponent.h"
#include "GameFramework/Character.h"
#include "STUBaseWeapon.h"

USTUWeaponComponent::USTUWeaponComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void USTUWeaponComponent::StartShooting()
{
    if (!CurrentWeapon)
    {
        return;
    }

    CurrentWeapon->Fire();
}

void USTUWeaponComponent::StopShooting()
{
    if (!CurrentWeapon)
    {
        return;
    }

    CurrentWeapon->StopFire();
}

void USTUWeaponComponent::BeginPlay()
{
    Super::BeginPlay();

    SpawnWeapon();
}

void USTUWeaponComponent::SpawnWeapon()
{
    auto Character = Cast<ACharacter>(GetOwner());
    
    if (!Character || !GetWorld())
    {
        return;
    }

    CurrentWeapon = GetWorld()->SpawnActor<ASTUBaseWeapon>(WeaponClass);
    if (!CurrentWeapon)
    {
        return;
    }
    
    FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
    CurrentWeapon->AttachToComponent(Character->GetMesh(), AttachmentRules, WeaponAttachPointName);
    CurrentWeapon->SetOwner(Character);
}