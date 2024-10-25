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
#if defined(__linux__)
        __asm__("nop");
#endif
    }
};

int main(int argc, char *argv[])
{
    Path::setWorkingPath(Path::getExecutablePath(argv[0]));

    Event<void(int)> OnEventExample;

    ExampleClass ExampleClassInstance;
    ExampleSubClassVirtual ExampleSubClassVirtualInstance;


    auto callback = Callback<void(int)>(
        // force functor to be possible to remove it
        (void (*)(int))[](int value) {
            printf("    [Callback] current value: %i\n", value);
        });
    OnEventExample.add(callback);
    OnEventExample.add(OnFunctionCall);
    OnEventExample.add(&ExampleClass::OnMethodCall, &ExampleClassInstance);
    OnEventExample.add(&ExampleClass::OnMethodCall, &ExampleSubClassVirtualInstance);
    // OnEventExample.add(&ExampleSubClassVirtual::OnMethodCall2, &ExampleSubClassVirtualInstance);

    OnEventExample.add([](int value)
                       { printf("    [Lambda] current value: %i\n", value); });

    int count = 0;
    printf("CALLING with count: %i\n", count);
    OnEventExample(count++);

    printf("(-) REMOVING ExampleClass::OnMethodCall\n");
    OnEventExample.remove(&ExampleClass::OnMethodCall, &ExampleClassInstance);
    printf("CALLING with count: %i\n", count);
    OnEventExample(count++);

    printf("(-) REMOVING ExampleClass::OnMethodCall (virtual ref)\n");
    OnEventExample.remove(&ExampleClass::OnMethodCall, &ExampleSubClassVirtualInstance);
    printf("CALLING with count: %i\n", count);
    OnEventExample(count++);

    // printf("(-) REMOVING ExampleSubClassVirtual::OnMethodCall2\n");
    // OnEventExample.remove(&ExampleSubClassVirtual::OnMethodCall2, &ExampleSubClassVirtualInstance);
    // printf("CALLING with count: %i\n", count);
    // OnEventExample(count++);

    printf("(-) REMOVING OnFunctionCall\n");
    OnEventExample.remove(OnFunctionCall);
    printf("CALLING with count: %i\n", count);
    OnEventExample(count++);

    printf("(-) REMOVING Callback\n");
    OnEventExample.remove(callback);
    printf("CALLING with count: %i\n", count);
    OnEventExample(count++);

    return 0;
}
