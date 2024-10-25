#include <InteractiveToolkit/InteractiveToolkit.h>
#include <iostream>

using namespace ITKCommon;
using namespace EventCore;

void OnFunctionCall(int value)
{
    printf("    [OnFunctionCall] current value: %i\n", value);
}

class ExampleClass : public HandleCallback
{
public:
    virtual void OnMethodCall(int value)
    {
        printf("    [ExampleClass::OnMethodCall] current value: %i\n", value);
    }
};


class ExampleSubClassVirtual : virtual public ExampleClass
{
public:
    void OnMethodCall(int value)
    {
        printf("    [ExampleSubClassVirtual::OnMethodCall] current value: %i\n", value);
    }


    void OnMethodCall2(int value)
    {
        printf("    [ExampleSubClassVirtual::OnMethodCall2] current value: %i\n", value);
    }

    void OnMethodCall3(int value)
    {
        __asm__("nop"); 
    }
};

int main(int argc, char *argv[])
{
    Path::setWorkingPath(Path::getExecutablePath(argv[0]));

    int count = 0;

    Callback<void(int)> callback;
    ExampleClass ExampleClassInstance;
    ExampleSubClassVirtual ExampleSubClassVirtualInstance;

    if (callback == nullptr)
        printf("callback is null\n");

    callback = OnFunctionCall;
    if (callback == OnFunctionCall)
        printf("callback is OnFunctionCall\n");
    callback(count++);

    callback = CallbackWrapper(&ExampleClass::OnMethodCall, &ExampleClassInstance);
    if (callback == CallbackWrapper(&ExampleClass::OnMethodCall, &ExampleClassInstance))
        printf("callback is ExampleClass::OnMethodCall\n");
    callback(count++);

    callback = CallbackWrapper(&ExampleClass::OnMethodCall, &ExampleSubClassVirtualInstance);
    if (callback == CallbackWrapper(&ExampleClass::OnMethodCall, &ExampleSubClassVirtualInstance))
        printf("callback is ExampleSubClassVirtual::OnMethodCall\n");
    callback(count++);

    // callback = CallbackWrapper(&ExampleSubClassVirtual::OnMethodCall2, &ExampleSubClassVirtualInstance);
    // if (callback == CallbackWrapper(&ExampleSubClassVirtual::OnMethodCall2, &ExampleSubClassVirtualInstance))
    //     printf("callback is ExampleSubClassVirtual::OnMethodCall2\n");
    // callback(count++);

    callback = [](int value)
    { printf("    [Lambda] current value: %i\n", value); };
    if (callback != nullptr)
        printf("callback is not null (Cannot compare lambda functions)\n");
    callback(count++);

    auto std_function = callback.toFunction();
    printf("Casting callback to a std::function\n");
    std_function(count++);

    callback = nullptr;
    if (callback == nullptr)
        printf("callback is null\n");

    {
        printf("Event as Callback\n");

        Event<void(int)> OnEventExample;
        OnEventExample.add([](int value)
                    { printf("    [Lambda] current value: %i\n", value); });
        callback = OnEventExample;
        callback(count++);
    }

    // callback = CallbackWrapper(&ExampleSubClassVirtual::OnMethodCall3, &ExampleSubClassVirtualInstance);

    // Platform::Time timer;
    // timer.update();
    // for(int64_t i = 0; i< INT64_C(99999999); i++){
    //     callback(i);
    // }
    // timer.update();

    // std::cout << "elapsed: " << timer.deltaTimeMicro << "us\n" << std::endl;
    
    return 0;
}
