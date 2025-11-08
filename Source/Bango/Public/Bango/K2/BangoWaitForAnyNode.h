#pragma once
#include "Kismet/BlueprintAsyncActionBase.h"

#include "BangoWaitForAnyNode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBangoDelayOutputPin);


#define DEFINE_PRIVATE_TYPE_ACCESSOR(Type1, Type2) \
struct Type1##_##Type2##_##_PrivateAccessTag\
{\
friend consteval auto* ResolvePrivateType(Type1##_##Type2##_##_PrivateAccessTag);\
};\
\
template struct TAccessPrivateType<Type1##_##Type2##_##_PrivateAccessTag, Type1::Type2>;\
using Type1##_##Type2##_Type = std::remove_pointer_t<decltype(ResolvePrivateType(Type1##_##Type2##_##_PrivateAccessTag{}))>;


template<typename Tag, typename T>
struct TAccessPrivateType
{
    friend consteval auto* ResolvePrivateType(Tag)
    {
        T* Ptr = nullptr;
        return Ptr;
    }
};


UCLASS()
class BANGO_API UK2Node_BangoWaitForAny : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()

    const UObject* WorldContextObject;
    
public:
    
    UPROPERTY(BlueprintAssignable, DisplayName = "Done")
    FBangoDelayOutputPin Completed;

    void Activate() override;
    
    UFUNCTION(BlueprintCallable, Category="Bango|Utilities", DisplayName = "Wait for Any", meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"))
    static UK2Node_BangoWaitForAny* Run(const UObject* WorldContextObject, float abc);

    UFUNCTION()
    void OnRunComplete();
};



DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDelayOneFrameOutputPin, float, InputFloatPlusOne, float, InputFloatPlusTwo);

UCLASS()
class UDelayOneFrame : public UBlueprintAsyncActionBase
{
    GENERATED_UCLASS_BODY()
public:
    UPROPERTY(BlueprintAssignable)
    FDelayOneFrameOutputPin AfterOneFrame;
    
    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Flow Control")
    static UDelayOneFrame* WaitForOneFrame(const UObject* WorldContextObject, const float SomeInputVariables);

    // UBlueprintAsyncActionBase interface
    virtual void Activate() override;
    //~UBlueprintAsyncActionBase interface
private:
    UFUNCTION()
    void ExecuteAfterOneFrame();

private:
    const UObject* WorldContextObject;
    float MyFloatInput;
};