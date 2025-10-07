#include "Components/STUWeaponComponent.h"
#include "GameFramework/Character.h"
#include "Animations/STUEquipWeponAnimNotify.h"
#include "STUBaseWeapon.h"

DEFINE_LOG_CATEGORY_STATIC(LogHWeaponComponent, All, All);

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
    InitAnimations();
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
    auto Character = GetCharacter();

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
    auto Character = GetCharacter();

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
    auto Character = GetCharacter();

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
    PlayAnimMontage(EquipAnimMontage);
}

void USTUWeaponComponent::PlayAnimMontage(UAnimMontage* AnimMontage)
{
    auto Character = GetCharacter();
    if (!Character)
    {
        return;
    }

    Character->PlayAnimMontage(AnimMontage);
}

void USTUWeaponComponent::InitAnimations()
{
    if (!EquipAnimMontage)
    {
        return;
    }
    const auto NotifyEvents = EquipAnimMontage->Notifies;

    for (auto NotifyEvent : NotifyEvents)
    {
        if (auto EquipFinishNotifyEvent = Cast<USTUEquipWeponAnimNotify>(NotifyEvent.Notify))
        {
            EquipFinishNotifyEvent->OnNotified.AddUObject(this, &USTUWeaponComponent::OnWeaponEquipFinished);
            break;
        }
    }
}

void USTUWeaponComponent::OnWeaponEquipFinished(USkeletalMeshComponent* MeshComponent)
{
    auto Character = GetCharacter();
    if (!Character)
    {
        return;
    }

    if (Character->GetMesh() == MeshComponent)
    {
        UE_LOG(LogHWeaponComponent, Display, TEXT("OnWeaponEquipFinished"));
    }
}

ACharacter* USTUWeaponComponent::GetCharacter()
{
    return Cast<ACharacter>(GetOwner());
}

void USTUWeaponComponent::SwitchWeapon()
{
    CurrentWeaponIndex = (CurrentWeaponIndex + 1) % Weapons.Num();
    EquipWeapon(CurrentWeaponIndex);
}
