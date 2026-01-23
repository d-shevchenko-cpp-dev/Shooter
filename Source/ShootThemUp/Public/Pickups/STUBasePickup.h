#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STUBasePickup.generated.h"

class USphereComponent;

UCLASS()
class SHOOTTHEMUP_API ASTUBasePickup : public AActor
{
    GENERATED_BODY()

public:
    ASTUBasePickup();

    virtual void Tick(float DeltaTime) override;

protected:
    virtual void BeginPlay() override;

    virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

private:
    virtual bool GivePickupTo(APawn* PlayerPawn);

    void PickupWasTaken();

    void Respawn();

protected:
    UPROPERTY(VisibleAnywhere, Category = "Pickup")
    USphereComponent* CollisionComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
    float RepawnTime = 5.0f;
};
