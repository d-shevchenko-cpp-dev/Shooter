#include "Components/STUWeaponComponent.h"
#include "STUAmmoComponent.h"
#include "GameFramework/Character.h"
#include "Animations/STUEquipWeaponAnimNotify.h"
#include "Animations/STUReloadFinishedAnimNotify.h"
#include "STUBaseWeapon.h"
#include "Weapon/STUWeaponTypes.h"

DEFINE_LOG_CATEGORY_STATIC(LogWeaponComponent, All, All);

USTUWeaponComponent::USTUWeaponComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool USTUWeaponComponent::GetWeaponUIData(FWeaponUIData& UIData) const
{
    if (CurrentWeapon)
    {
        UIData = CurrentWeapon->GetUIData();
        return true;
    }
    return false;
}

void USTUWeaponComponent::BeginPlay()
{
    Super::BeginPlay();
    InitAnimations();
    SpawnWeapons();
    AttachWeaponsToActor();
    EquipWeapon(CurrentWeaponIndex);
    for (const auto Weapon : Weapons)
    {
        Weapon->GetAmmoComponent()->OnClipEmpty.AddUObject(this, &USTUWeaponComponent::Reload);
    }
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

void USTUWeaponComponent::StartShooting()
{
    if (!IsWeaponReadyForAction())
    {
        return;
    }

    CurrentWeapon->StartFire();
}

void USTUWeaponComponent::StopShooting()
{
    if (!CurrentWeapon)
    {
        return;
    }

    CurrentWeapon->StopFire();
}

void USTUWeaponComponent::SpawnWeapons()
{
    auto Character = GetCharacter();
    check(GetWorld());

    for (const auto& Data : WeaponData)
    {
        auto Weapon = GetWorld()->SpawnActor<ASTUBaseWeapon>(Data.WeaponClass);
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
    for (auto Weapon : Weapons)
    {
        Weapon->SetOwner(Character);
        AttachWeaponToSocket(GetCharacterMesh(), Weapon, WeaponArmorySocketName);
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
    if (WeaponIndex < 0 || WeaponIndex >= Weapons.Num())
    {
        UE_LOG(LogWeaponComponent, Warning, TEXT("Invalid Weapon Index"));
        return;
    }

    if (CurrentWeapon)
    {
        StopShooting();
        AttachWeaponToSocket(GetCharacterMesh(), CurrentWeapon, WeaponArmorySocketName);
    }

    CurrentWeapon = Weapons[WeaponIndex];

    AttachWeaponToSocket(GetCharacterMesh(), CurrentWeapon, WeaponEquipSocketName);
    bAnimInProcess = true;
    PlayAnimMontage(EquipAnimMontage);
}

void USTUWeaponComponent::PlayAnimMontage(UAnimMontage* AnimMontage)
{
    GetCharacter()->PlayAnimMontage(AnimMontage);
}

void USTUWeaponComponent::InitAnimations()
{
    if (auto EquipFinishedNotify = FindNotifyByClass<USTUEquipWeaponAnimNotify>(EquipAnimMontage))
    {
        EquipFinishedNotify->OnNotified.AddUObject(this, &USTUWeaponComponent::OnWeaponEquipFinished);
    }

    for (const auto& Data : WeaponData)
    {
        if (auto ReloadFinishedNotify = FindNotifyByClass<USTUReloadFinishedAnimNotify>(Data.ReloadAnimMontage))
        {
            ReloadFinishedNotify->OnNotified.AddUObject(this, &USTUWeaponComponent::OnReloadFinished);
        }
    }
}

void USTUWeaponComponent::Reload()
{
    if (!IsWeaponReadyForAction())
    {
        return;
    }

    if (CurrentWeapon && CurrentWeapon->GetAmmoComponent())
    {
        if (!CurrentWeapon->GetAmmoComponent()->CanReload())
        {
            UE_LOG(LogWeaponComponent, Warning, TEXT("Cannot reload: clip is full or no clips available"));
            return;
        }
    }

    bAnimInProcess = true;
    PlayAnimMontage(GetReloadAnimMontage(CurrentWeapon));
}

void USTUWeaponComponent::SwitchWeapon()
{
    if (!IsWeaponReadyForAction())
    {
        return;
    }
    CurrentWeaponIndex = (CurrentWeaponIndex + 1) % Weapons.Num();
    EquipWeapon(CurrentWeaponIndex);
}

void USTUWeaponComponent::OnWeaponEquipFinished(USkeletalMeshComponent* MeshComponent)
{
    if (MeshComponent != GetCharacterMesh())
    {
        return;
    }

    bAnimInProcess = false;
}

void USTUWeaponComponent::OnReloadFinished(USkeletalMeshComponent* MeshComponent)
{
    if (MeshComponent != GetCharacterMesh())
    {
        return;
    }

    if (CurrentWeapon && CurrentWeapon->GetAmmoComponent())
    {
        CurrentWeapon->GetAmmoComponent()->TryReload();
    }

    bAnimInProcess = false;
}

ACharacter* USTUWeaponComponent::GetCharacter() const
{
    auto Character = Cast<ACharacter>(GetOwner());
    check(Character);
    return Character;
}

USkeletalMeshComponent* USTUWeaponComponent::GetCharacterMesh() const
{
    return GetCharacter()->GetMesh();
}

UAnimMontage* USTUWeaponComponent::GetReloadAnimMontage(ASTUBaseWeapon* Weapon) const
{
    const auto CurrentWeaponData =
        WeaponData.FindByPredicate([&](const auto& Data) { return Data.WeaponClass == Weapon->GetClass(); });

    return CurrentWeaponData ? CurrentWeaponData->ReloadAnimMontage : nullptr;
}

bool USTUWeaponComponent::IsAnimationInProcess() const
{
    return bAnimInProcess;
}

bool USTUWeaponComponent::IsWeaponReadyForAction() const
{
    return CurrentWeapon && !IsAnimationInProcess();
}
