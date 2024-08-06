
#pragma once

#include "RaycastNotifyable.generated.h"

//        To inherit from this class: simply append it to the list of includes; for example:
//    class UMyExampleClass : public AActor, public IRaycastNotifyable {...}
//
//        Then make sure you supply any URaycastComponent line trace's final argument with "true" to enable notifying hit actors
//    pRaycastComponent->AnyRaycastFunction(..., true);

//Required by the engine. Only used for checking if implemented.
UINTERFACE(MinimalAPI)
class URaycastNotifyable : public UInterface
{
    GENERATED_BODY()
};

class IRaycastNotifyable
{
    GENERATED_BODY()
public:
    /*    Notify this actor of a raycast hit.
        This method MUST be implemented by child classes.
        const AActor* instigator: The actor that owns the raycast;
        const FHitResult* hit: A pointer to a hit result. Can be, and is, nullptr by default if the hit result is not important;
        RETURNS nothing.
    */
    virtual void NotifyHit(AActor* instigator, const FHitResult* hit = nullptr) = 0;
};