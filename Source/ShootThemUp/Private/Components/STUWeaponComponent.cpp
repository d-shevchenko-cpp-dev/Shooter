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

    SpawnWeapons();
    AttachWeaponsToActor();
    EquipWeapon(CurrentWeaponIndex);
}

void USTUWeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    CurrentWeapon = nullptr;
    for (auto Weapon : Weapons)
    {
        Weapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
        Weapon->Destroy();
    }
    Weapons.Empty();
    Super::EndPlay(EndPlayReason);
}

void USTUWeaponComponent::SpawnWeapons()
{
    auto Character = Cast<ACharacter>(GetOwner());

    if (!GetWorld() || !Character)
    {
        return;
    }

    for (auto WeaponClass : WeaponClasses)
    {
        auto Weapon = GetWorld()->SpawnActor<ASTUBaseWeapon>(WeaponClass);
        if (!Weapon)
        {
            continue;
        }

        Weapon->SetOwner(Character);
        Weapons.Add(Weapon);
    }
}

void USTUWeaponComponent::AttachWeaponsToActor()
{
    auto Character = Cast<ACharacter>(GetOwner());

    if (!Character)
    {
        return;
    }

    for (auto Weapon : Weapons)
    {
        Weapon->SetOwner(Character);

        FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
        Weapon->AttachToComponent(Character->GetMesh(), AttachmentRules, WeaponArmorySocketName);
    }
}

void USTUWeaponComponent::AttachWeaponToSocket(USkeletalMeshComponent* Mesh,
    ASTUBaseWeapon* Weapon,
    const FName& SocketName)
{
    FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
    Weapon->AttachToComponent(Mesh, AttachmentRules, SocketName);
}

void USTUWeaponComponent::EquipWeapon(int32 WeaponIndex)
{
    auto Character = Cast<ACharacter>(GetOwner());

    if (!Character)
    {
        return;
    }

    if (CurrentWeapon)
    {
        StopShooting();
        AttachWeaponToSocket(Character->GetMesh(), CurrentWeapon, WeaponArmorySocketName);
    }

    if (!Weapons.Num())
    {
        UE_LOG(LogTemp, Warning, TEXT("USTUWeaponComponent::EquipWeapon Weapons array is empty!"))
        return;
    }
    CurrentWeapon = Weapons[WeaponIndex];
    AttachWeaponToSocket(Character->GetMesh(), CurrentWeapon, WeaponEquipSocketName);
}

void USTUWeaponComponent::SwitchWeapon()
{
    CurrentWeaponIndex = (CurrentWeaponIndex + 1) % Weapons.Num();
    EquipWeapon(CurrentWeaponIndex);
}
