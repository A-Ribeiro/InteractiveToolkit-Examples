#include <InteractiveToolkit/InteractiveToolkit.h>
#include <iostream>

using namespace ITKCommon;
using namespace EventCore;
using namespace MathCore;


int main(int argc, char *argv[])
{
    Path::setWorkingPath(Path::getExecutablePath(argv[0]));


    printf("\nProperty\n\n");

    Property<vec2f> propertyVec2f;
    propertyVec2f.OnChange.add([](const vec2f&value, const vec2f&oldValue){
        printf("replacing value: %f %f\n", oldValue.x, oldValue.y);
        printf("by new value: %f %f\n", value.x, value.y);
    });

    printf("Property<vec2f>\n");
    propertyVec2f = vec2f(35.5f, 20.6f);
    propertyVec2f = propertyVec2f + vec2f(.2f, .3f);
    printf("\n");


    Property<bool> propertyBool;
    propertyBool.OnChange.add([](const bool&value, const bool&oldValue){
        printf("replacing value: %i\n", oldValue);
        printf("by new value: %i\n", value);
    });

    printf("Property<bool>\n");
    propertyBool = true;
    propertyBool = propertyBool && (5 < 2);
    printf("\n");


    printf("\nVirtualProperty\n\n");
    vec2f v = vec2f(-1,-1);
    VirtualProperty<vec2f> virtPropVec2f(
        //get
        [&v](){
            printf("[VirtualProperty]get: %f %f\n", v.x, v.y);
            return v;
        },
        //set
        [&v](const vec2f & _v){
            v = _v;
            printf("[VirtualProperty]set: %f %f\n", v.x, v.y);
        }
    );

    virtPropVec2f = virtPropVec2f * vec2f(2.0f);

    printf("\n");

    return 0;
}
