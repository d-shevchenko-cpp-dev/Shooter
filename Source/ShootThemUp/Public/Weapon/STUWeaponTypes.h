#pragma once

#include "CoreMinimal.h"
#include "STUWeaponTypes.generated.h"

class UTexture2D;

USTRUCT(BlueprintType)
struct FWeaponUIData
{
    GENERATED_BODY();

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    UTexture2D* MainIcon = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    UTexture2D* CrossHairIcon = nullptr;
};
