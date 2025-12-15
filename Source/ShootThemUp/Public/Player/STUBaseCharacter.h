#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "STUBaseCharacter.generated.h"

class UTextRenderComponent;
class USTUHealthComponent;
class USpringArmComponent;
class UCameraComponent;
class USTUWeaponComponent;

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
    /** Возвращает true, если персонаж в данный момент бежит */
    UFUNCTION(BlueprintCallable, Category = "Movement")
    bool IsRunning() const;

    /** Возвращает угол направления движения в градусах */
    UFUNCTION(BlueprintCallable, Category = "Movement")
    float GetMovementDirection() const;

private:
    // Обработчики ввода движения
    void MoveForward(float Amount);
    void MoveRight(float Amount);
    void OnStartRunning();
    void OnStopRunning();

    // Обработчики здоровья и смерти
    void OnDeath();
    void OnHealthChanged(float Health);

    // Обработчик урона от приземления
    UFUNCTION()
    void OnGroundLanded(const FHitResult& Hit);

protected:
    // Компоненты камеры и движения
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
    UCameraComponent* CameraComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
    USpringArmComponent* SpringArmComponent;

    // Компоненты системы здоровья
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
    USTUHealthComponent* HealthComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
    UTextRenderComponent* HealthTextComponent;

    // Компонент системы оружия
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
    USTUWeaponComponent* WeaponComponent;

    // Настройки анимации
    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    UAnimMontage* DeathAnimMontage;

    // Настройки урона от приземления
    UPROPERTY(EditDefaultsOnly,
        Category = "Movement",
        meta = (ToolTip = "Velocity range for landing damage calculation (min, max)"))
    FVector2D LandedDamageVelocity{ FVector2D(900.f, 1200.f) };

    UPROPERTY(EditDefaultsOnly, Category = "Movement", meta = (ToolTip = "Damage range for landing damage (min, max)"))
    FVector2D LandedDamage{ FVector2D(10.f, 100.f) };

private:
    bool IsCharacterRunning{ false };
    bool IsCharacterMoving{ false };

    static constexpr float DEATH_LIFESPAN = 5.0f;
    static constexpr float SPRING_ARM_SOCKET_OFFSET_Y = 100.0f;
    static constexpr float SPRING_ARM_SOCKET_OFFSET_Z = 80.0f;
};
