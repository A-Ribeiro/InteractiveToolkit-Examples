#include "common.h"

#include "vec.h"
#include "mat.h"

int main(int argc, char *argv[])
{
    Path::setWorkingPath(Path::getExecutablePath(argv[0]));

    /*

    quat<float, SIMD_TYPE::NONE> a(
        0.1f,0.2f,
        0,0
    );
    quat<float, SIMD_TYPE::NONE> b(
        0.1f,0.2f,
        0,0
    );

    printf("%i\n", (int)(a == b) );

    //*/

    //*
    test_vec2();
    test_vec3();
    test_vec4();
    test_quat();
    test_mat2();
    test_mat3();
    test_mat4();

    // */

    return 0;
}
