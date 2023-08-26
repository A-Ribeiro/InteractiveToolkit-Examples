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

    Event<void(int)> OnEventExample;

    ExampleClass ExampleClassInstance;

    OnEventExample.add(OnFunctionCall);
    OnEventExample.add(&ExampleClass::OnMethodCall, &ExampleClassInstance);
    OnEventExample.add([](int value)
                       { printf("    [Lambda] current value: %i\n", value); });

    int count = 0;
    printf("CALLING with count: %i\n", count);
    OnEventExample(count++);

    printf("(-) REMOVING ExampleClass::OnMethodCall\n");
    OnEventExample.remove(&ExampleClass::OnMethodCall, &ExampleClassInstance);
    printf("CALLING with count: %i\n", count);
    OnEventExample(count++);

    printf("(-) REMOVING OnFunctionCall\n");
    OnEventExample.remove(OnFunctionCall);
    printf("CALLING with count: %i\n", count);
    OnEventExample(count++);

    return 0;
}
