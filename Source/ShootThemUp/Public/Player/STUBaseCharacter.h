// ShootThemUp Game. All Right Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "STUBaseCharacter.generated.h"

class UTextRenderComponent;
class USTUHealthComponent;
class USpringArmComponent;
class UCameraComponent;
class USTUWeaponComponent;

/**
 * Base character class for ShootThemUp game.
 * Provides common functionality for all characters including movement, health, and weapon systems.
 */

UCLASS()
class SHOOTTHEMUP_API ASTUBaseCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ASTUBaseCharacter(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
    /** Returns true if character is currently running */
    UFUNCTION(BlueprintCallable, Category="Movement")
    bool IsRunning() const;

    /** Returns the movement direction angle in degrees */
    UFUNCTION(BlueprintCallable, Category="Movement")
    float GetMovementDirection() const;

private:
    // Movement input handlers
    void MoveForward(float Amount);
    void MoveRight(float Amount);
    void OnStartRunning();
    void OnStopRunning();

    // Health and death handlers
    void OnDeath();
    void OnHealthChanged(float Health);
    
    // Landing damage handler
    UFUNCTION()
    void OnGroundLanded(const FHitResult& Hit);

protected:
    // Camera and movement components
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
    UCameraComponent* CameraComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
    USpringArmComponent* SpringArmComponent;

    // Health system components
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
    USTUHealthComponent* HealthComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
    UTextRenderComponent* HealthTextComponent;

    // Weapon system component
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
    USTUWeaponComponent* WeaponComponent;

    // Animation settings
    UPROPERTY(EditDefaultsOnly, Category="Animation")
    UAnimMontage* DeathAnimMontage;

    // Landing damage settings
    UPROPERTY(EditDefaultsOnly, Category="Movement", meta=(ToolTip="Velocity range for landing damage calculation (min, max)"))
    FVector2D LandedDamageVelocity{FVector2D(900.f, 1200.f)};

    UPROPERTY(EditDefaultsOnly, Category="Movement", meta=(ToolTip="Damage range for landing damage (min, max)"))
    FVector2D LandedDamage{FVector2D(10.f, 100.f)};

private:
    // Movement state flags
    bool IsCharacterRunning{false};
    bool IsCharacterMoving{false};

    // Constants
    static constexpr float DEATH_LIFESPAN = 5.0f;
    static constexpr float SPRING_ARM_SOCKET_OFFSET_Y = 100.0f;
    static constexpr float SPRING_ARM_SOCKET_OFFSET_Z = 80.0f;
};