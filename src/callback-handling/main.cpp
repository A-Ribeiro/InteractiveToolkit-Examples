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
    void OnMethodCall(int value)
    {
        printf("    [ExampleClass::OnMethodCall] current value: %i\n", value);
    }
};

int main(int argc, char *argv[])
{
    Path::setWorkingPath(Path::getExecutablePath(argv[0]));

    int count = 0;

    Callback<void(int)> callback;
    ExampleClass ExampleClassInstance;

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
    
    return 0;
}
