// ShootThemUp Game. All Right Reserved.


#include "Weapon/STUBaseWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "engine/World.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogBaseWeapon, All, All);

ASTUBaseWeapon::ASTUBaseWeapon()
{
    PrimaryActorTick.bCanEverTick = false;

    WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
}

void ASTUBaseWeapon::Fire()
{
    UE_LOG(LogBaseWeapon, Display, TEXT("Fire"));

    MakeShot();
}

// Called when the game starts or when spawned
void ASTUBaseWeapon::BeginPlay()
{
    Super::BeginPlay();
    check(WeaponMesh);
}

void ASTUBaseWeapon::MakeShot()
{
    if (!IsValidForShooting())
    {
        return;
    }

    const ACharacter* Player = Cast<ACharacter>(GetOwner());
    const APlayerController* PlayerController = Player->GetController<APlayerController>();
    
    // Get camera position and direction
    FVector CameraLocation;
    FRotator CameraRotation;
    PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);
    
    // Determine trace points
    const FTransform SocketTransform = WeaponMesh->GetSocketTransform(MuzzleSocketName);
    const FVector TraceStart = SocketTransform.GetLocation();
    const FVector TraceEnd = CameraLocation + CameraRotation.Vector() * MaxRange;

    // Perform line trace
    FHitResult HitResult;
    PerformLineTrace(TraceStart, TraceEnd, HitResult);

    // Display debug information
    if (bDrawDebugTrace)
    {
        DrawDebugTrace(TraceStart, TraceEnd, HitResult);
    }

    // Process hit result
    if (HitResult.bBlockingHit)
    {
        UE_LOG(LogBaseWeapon, Display, TEXT("Hit target at bone: %s"), *HitResult.BoneName.ToString());
    }
}

bool ASTUBaseWeapon::IsValidForShooting() const
{
    if (!GetWorld())
    {
        UE_LOG(LogBaseWeapon, Warning, TEXT("World is not valid"));
        return false;
    }

    if (!WeaponMesh)
    {
        UE_LOG(LogBaseWeapon, Warning, TEXT("WeaponMesh is not valid"));
        return false;
    }

    const ACharacter* Player = Cast<ACharacter>(GetOwner());
    if (!Player)
    {
        UE_LOG(LogBaseWeapon, Warning, TEXT("Owner is not a Character"));
        return false;
    }

    const APlayerController* PlayerController = Player->GetController<APlayerController>();
    if (!PlayerController)
    {
        UE_LOG(LogBaseWeapon, Warning, TEXT("PlayerController is not valid"));
        return false;
    }

    return true;
}

void ASTUBaseWeapon::PerformLineTrace(const FVector& TraceStart, const FVector& TraceEnd, FHitResult& HitResult) const
{
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(GetOwner());
    QueryParams.bReturnPhysicalMaterial = true;

    GetWorld()->LineTraceSingleByChannel(
        HitResult, 
        TraceStart, 
        TraceEnd, 
        ECollisionChannel::ECC_Visibility, 
        QueryParams
    );
}

void ASTUBaseWeapon::DrawDebugTrace(const FVector& TraceStart, const FVector& TraceEnd, const FHitResult& HitResult) const
{
    // Draw trace line
    DrawDebugLine(
        GetWorld(), 
        TraceStart, 
        TraceEnd, 
        FColor::Red, 
        false, 
        DebugTraceDuration, 
        0, 
        3.0f
    );

    // If hit target, draw sphere at impact point
    if (HitResult.bBlockingHit)
    {
        DrawDebugSphere(
            GetWorld(), 
            HitResult.ImpactPoint, 
            DebugHitSphereRadius, 
            24, 
            FColor::Red, 
            false, 
            DebugTraceDuration
        );
    }
}