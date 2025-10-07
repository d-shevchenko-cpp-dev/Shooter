// ShootThemUp Game. All Right Reserved.

#include "Animations/STUEquipWeponAnimNotify.h"

void USTUEquipWeponAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    OnNotified.Broadcast(MeshComp);
    Super::Notify(MeshComp, Animation);
}
