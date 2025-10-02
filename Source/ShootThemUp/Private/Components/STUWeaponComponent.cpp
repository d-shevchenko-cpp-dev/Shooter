// ShootThemUp Game. All Right Reserved.

#include "Components/STUWeaponComponent.h"
#include "GameFramework/Character.h"
#include "STUBaseWeapon.h"

USTUWeaponComponent::USTUWeaponComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void USTUWeaponComponent::Fire()
{
    if (!CurrentWeapon)
    {
        return;
    }

    CurrentWeapon->Fire();
}

void USTUWeaponComponent::BeginPlay()
{
    Super::BeginPlay();

    SpawnWeapon();
}

void USTUWeaponComponent::SpawnWeapon()
{
    if (!GetWorld())
    {
        return;
    }

    auto Character = Cast<ACharacter>(GetOwner());
    if (!Character)
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
}