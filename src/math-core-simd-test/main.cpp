#include <InteractiveToolkit/InteractiveToolkit.h>
#include <iostream>

using namespace ITKCommon;
using namespace MathCore;

template <typename T,
          typename std::enable_if<
              MathTypeInfo<T>::_is_valid::value &&
                  std::is_floating_point<typename MathTypeInfo<T>::_type>::value && //
                  !MathTypeInfo<T>::_is_vec::value                                  //
              ,
              bool>::type = true>
void print_any_vec(const T &v)
{
    printf("\n");
    for (int r = 0; r < (int)T::rows; r++)
    {
        printf("        [");
        for (int c = 0; c < (int)T::cols; c++)
            if (v[c][r] >= 0.0f)
                printf(" %e ", v[c][r]);
            else
                printf("%e ", v[c][r]);
        printf("]\n");
    }
    printf("        ");
}

template <typename T,
          typename std::enable_if<
              MathTypeInfo<T>::_is_valid::value &&
                  std::is_floating_point<typename MathTypeInfo<T>::_type>::value && //
                  MathTypeInfo<T>::_is_vec::value                                   //
              ,
              bool>::type = true>
void print_any_vec(const T &v)
{
    printf("( ");
    for (int i = 0; i < T::array_count; i++)
        if (v.array[i] >= 0.0f)
            printf(" %e ", v.array[i]);
        else
            printf("%e ", v.array[i]);
    printf(")");
}

template <typename T,
          typename std::enable_if<
              MathTypeInfo<T>::_is_valid::value &&
                  std::is_floating_point<typename MathTypeInfo<T>::_type>::value && //
                  !MathTypeInfo<T>::_is_vec::value                                  //
              ,
              bool>::type = true>
bool compare_any_vec(const T &a, const T &b)
{
    bool passed = true;
    for (int c = 0; c < (int)T::cols; c++){
        for (int r = 0; r < (int)T::rows; r++){
            passed = passed && OP<float>::compare_almost_equal(a[c][r], b[c][r]);
        }
    }
    return passed;
}

template <typename T,
          typename std::enable_if<
              MathTypeInfo<T>::_is_valid::value &&
                  std::is_floating_point<typename MathTypeInfo<T>::_type>::value && //
                  MathTypeInfo<T>::_is_vec::value                                   //
              ,
              bool>::type = true>
bool compare_any_vec(const T &a, const T &b)
{
    bool passed = true;
    for (int i = 0; i < T::array_count; i++)
        passed = passed && OP<float>::compare_almost_equal(a.array[i], b.array[i]);
    return passed;
}

template <typename T1, typename T2>
void test_vec(const std::string &name, const T1 &ref, const T2 &computed)
{
    const T1 computed_same_type = (const T1)computed;

    bool passed = (ref == computed_same_type);//compare_any_vec(ref, computed_same_type);

    if (passed)
    {
        printf("    %s ", name.c_str());
        print_any_vec(ref);
        printf(" == ");
        print_any_vec(computed);
        printf(" -> passed\n");
    }
    else
    {
        printf("    %s ", name.c_str());
        print_any_vec(ref);
        printf(" == ");
        print_any_vec(computed);
        printf(" -> failed\n");
        exit(-1);
    }
}

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

    {
        using base_type = vec2<float, SIMD_TYPE::NONE>;
        using simd_type = vec2f;
        base_type ref;
        simd_type target;

        MathRandomExt<Random> mathRnd(Random32::Instance());

        printf("\n[vec2f internal]\n\n");

        const base_type a = mathRnd.nextDirection<base_type>();
        const base_type b = mathRnd.nextDirection<base_type>();
        const base_type c = mathRnd.nextDirection<base_type>();
        const base_type d = mathRnd.nextDirection<base_type>();
        printf("  a = ");
        print_any_vec(a);
        printf("\n");
        printf("  b = ");
        print_any_vec(b);
        printf("\n");
        printf("  c = ");
        print_any_vec(c);
        printf("\n");
        printf("  d = ");
        print_any_vec(d);
        printf("\n");

        test_vec("vec2()", ref, (simd_type)ref);

        ref = mathRnd.nextDirection<base_type>();
        target = simd_type(ref.x, ref.y);
        test_vec("vec2( random )", ref, target);

        ref = base_type(a, b);
        target = simd_type((simd_type)a, (simd_type)b);
        test_vec("vec2( a, b )", ref, target);

        ref = (base_type)a += (base_type)b;
        target = (simd_type)a += (simd_type)b;
        test_vec("vec2( a += b )", ref, target);

        ref = (base_type)a -= (base_type)b;
        target = (simd_type)a -= (simd_type)b;
        test_vec("vec2( a -= b )", ref, target);

        ref = -a;
        target = -(simd_type)a;
        test_vec("vec2( -a )", ref, target);

        ref = (base_type)a *= (base_type)b;
        target = (simd_type)a *= (simd_type)b;
        test_vec("vec2( a *= b )", ref, target);

        ref = (base_type)a /= (base_type)b;
        target = (simd_type)a /= (simd_type)b;
        test_vec("vec2( a /= b )", ref, target);

        ref = (base_type)a += (float)b.x;
        target = (simd_type)a += (float)b.x;
        test_vec("vec2( a += b.x )", ref, target);

        ref = (base_type)a -= (float)b.x;
        target = (simd_type)a -= (float)b.x;
        test_vec("vec2( a -= b.x )", ref, target);

        ref = (base_type)a *= (float)b.x;
        target = (simd_type)a *= (float)b.x;
        test_vec("vec2( a *= b.x )", ref, target);

        ref = (base_type)a /= (float)b.x;
        target = (simd_type)a /= (float)b.x;
        test_vec("vec2( a /= b.x )", ref, target);

        printf("\n[vec2f op]\n\n");

        ref = OP<base_type>::clamp(a, -0.7f, 0.7f);
        target = OP<simd_type>::clamp((simd_type)a, -0.7f, 0.7f);
        test_vec("clamp( a, -0.7, 0.7 )", ref, target);

        ref = OP<base_type>::dot(a, b);
        target = OP<simd_type>::dot((simd_type)a, (simd_type)b);
        test_vec("dot( a, b )", ref, target);

        ref = OP<base_type>::normalize(a * 5.0f);
        target = OP<simd_type>::normalize((simd_type)a * 5.0f);
        test_vec("normalize( a * 5.0 )", ref, target);

        ref = OP<base_type>::angleBetween(a, b);
        target = OP<simd_type>::angleBetween((simd_type)a, (simd_type)b);
        test_vec("angleBetween( a, b )", ref, target);

        ref = OP<base_type>::cross_z_result(a, b);
        target = OP<simd_type>::cross_z_result((simd_type)a, (simd_type)b);
        test_vec("cross_z_result( a, b )", ref, target);

        ref = OP<base_type>::cross_z_up(a);
        target = OP<simd_type>::cross_z_up((simd_type)a);
        test_vec("cross_z_up( a )", ref, target);

        ref = OP<base_type>::cross_z_down(a);
        target = OP<simd_type>::cross_z_down((simd_type)a);
        test_vec("cross_z_down( a )", ref, target);

        ref = OP<base_type>::reflect(a, b);
        target = OP<simd_type>::reflect((simd_type)a, (simd_type)b);
        test_vec("reflect( a, b )", ref, target);

        ref = OP<base_type>::refract(a, b, 0.5, 0.7);
        target = OP<simd_type>::refract((simd_type)a, (simd_type)b, 0.5, 0.7);
        test_vec("refract( a, b, 0.5, 0.7 )", ref, target);

        ref = OP<base_type>::sqrLength(a * 2.0f);
        target = OP<simd_type>::sqrLength((simd_type)a * 2.0f);
        test_vec("sqrLength( a  * 2.0 )", ref, target);

        ref = OP<base_type>::length(a * 2.0f);
        target = OP<simd_type>::length((simd_type)a * 2.0f);
        test_vec("length( a  * 2.0 )", ref, target);

        ref = OP<base_type>::sqrDistance(a, b);
        target = OP<simd_type>::sqrDistance((simd_type)a, (simd_type)b);
        test_vec("sqrDistance( a, b )", ref, target);

        ref = OP<base_type>::distance(a, b);
        target = OP<simd_type>::distance((simd_type)a, (simd_type)b);
        test_vec("distance( a, b )", ref, target);

        ref = OP<base_type>::parallelComponent(a, b);
        target = OP<simd_type>::parallelComponent((simd_type)a, (simd_type)b);
        test_vec("parallelComponent( a, b )", ref, target);

        ref = OP<base_type>::perpendicularComponent(a, b);
        target = OP<simd_type>::perpendicularComponent((simd_type)a, (simd_type)b);
        test_vec("perpendicularComponent( a, b )", ref, target);

        base_type r1, r2;
        OP<base_type>::vecDecomp(a, b, &r1, &r2);

        simd_type r1_s, r2_s;
        OP<simd_type>::vecDecomp(a, b, &r1_s, &r2_s);

        ref = r1 + r2;
        target = r1_s + r2_s;
        test_vec("vecDecomp( a, b, &r1, &r2 )", ref, target);

        ref = OP<base_type>::quadraticClamp(a, b, 0.7f);
        target = OP<simd_type>::quadraticClamp((simd_type)a, (simd_type)b, 0.7f);
        test_vec("quadraticClamp( a, b, 0.7 )", ref, target);

        ref = OP<base_type>::maximum(a);
        target = OP<simd_type>::maximum((simd_type)a);
        test_vec("maximum( a )", ref, target);

        ref = OP<base_type>::maximum(a, b);
        target = OP<simd_type>::maximum((simd_type)a, (simd_type)b);
        test_vec("maximum( a, b )", ref, target);

        ref = OP<base_type>::minimum(a);
        target = OP<simd_type>::minimum((simd_type)a);
        test_vec("minimum( a )", ref, target);

        ref = OP<base_type>::minimum(a, b);
        target = OP<simd_type>::minimum((simd_type)a, (simd_type)b);
        test_vec("minimum( a, b )", ref, target);

        ref = OP<base_type>::abs(a);
        target = OP<simd_type>::abs((simd_type)a);
        test_vec("abs( a )", ref, target);

        ref = OP<base_type>::lerp(a, b, 0.75f);
        target = OP<simd_type>::lerp((simd_type)a, (simd_type)b, 0.75f);
        test_vec("lerp( a, b, 0.75 )", ref, target);

        ref = OP<base_type>::slerp(a, b, 0.75f);
        target = OP<simd_type>::slerp((simd_type)a, (simd_type)b, 0.75f);
        test_vec("slerp( a, b, 0.75 )", ref, target);

        ref = OP<base_type>::barylerp(0.2f, 0.5f, a, b, 0.75f);
        target = OP<simd_type>::barylerp(0.2f, 0.5f, (simd_type)a, (simd_type)b, 0.75f);
        test_vec("barylerp( 0.2, 0.5, a, b, 0.75 )", ref, target);

        ref = OP<base_type>::blerp(a, a + b, a + b + c, a + c, 0.75f, 0.75f);
        target = OP<simd_type>::blerp((simd_type)a, (simd_type)a + (simd_type)b, (simd_type)a + (simd_type)b + (simd_type)c, (simd_type)a + (simd_type)c, 0.75f, 0.75f);
        test_vec("blerp( a, a + b, a + b + c, a + c, 0.75, 0.75 )", ref, target);

        ref = OP<base_type>::splineCatmullRom(a, b, c, d, 0.75f);
        target = OP<simd_type>::splineCatmullRom((simd_type)a, (simd_type)b, (simd_type)c, (simd_type)d, 0.75f);
        test_vec("splineCatmullRom( a, b, c, d, 0.75 )", ref, target);

        ref = OP<base_type>::move(a, b, 0.3f);
        target = OP<simd_type>::move((simd_type)a, (simd_type)b, 0.3f);
        test_vec("move( a, b, 0.3 )", ref, target);

        ref = OP<base_type>::moveSlerp(a, b, 0.3f);
        target = OP<simd_type>::moveSlerp((simd_type)a, (simd_type)b, 0.3f);
        test_vec("moveSlerp( a, b, 0.3 )", ref, target);

        ref = OP<base_type>::sign(a);
        target = OP<simd_type>::sign((simd_type)a);
        test_vec("sign( a )", ref, target);

        ref = OP<base_type>::floor(a * 2.0f);
        target = OP<simd_type>::floor((simd_type)a * 2.0f);
        test_vec("floor( a * 2.0 )", ref, target);

        ref = OP<base_type>::ceil(a * 2.0f);
        target = OP<simd_type>::ceil((simd_type)a * 2.0f);
        test_vec("ceil( a * 2.0 )", ref, target);

        ref = OP<base_type>::round(a * 2.0f);
        target = OP<simd_type>::round((simd_type)a * 2.0f);
        test_vec("round( a * 2.0 )", ref, target);

        ref = OP<base_type>::fmod(a * 2.0f, b);
        target = OP<simd_type>::fmod((simd_type)a * 2.0f, (simd_type)b);
        test_vec("fmod( a * 2.0, b )", ref, target);

        ref = OP<base_type>::step(a * 2.0f, b);
        target = OP<simd_type>::step((simd_type)a * 2.0f, (simd_type)b);
        test_vec("step( a * 2.0, b )", ref, target);

        ref = OP<base_type>::smoothstep(a, b, c);
        target = OP<simd_type>::smoothstep((simd_type)a, (simd_type)b, (simd_type)c);
        test_vec("smoothstep( a, b, c )", ref, target);

        printf("\n[vec2f gen]\n\n");

        ref = GEN<base_type>::fromPolar(a.x * 360.0f, a.y);
        target = GEN<simd_type>::fromPolar(a.x * 360.0f, a.y);
        test_vec("fromPolar( a.x * 360.0, a.y )", ref, target);
    }

    {
        using base_type = vec3<float, SIMD_TYPE::NONE>;
        using simd_type = vec3f;
        base_type ref;
        simd_type target;

        MathRandomExt<Random> mathRnd(Random32::Instance());

        printf("\n[vec3f internal]\n\n");

        const base_type a = mathRnd.nextDirection<base_type>();
        const base_type b = mathRnd.nextDirection<base_type>();
        const base_type c = mathRnd.nextDirection<base_type>();
        const base_type d = mathRnd.nextDirection<base_type>();
        printf("  a = ");
        print_any_vec(a);
        printf("\n");
        printf("  b = ");
        print_any_vec(b);
        printf("\n");
        printf("  c = ");
        print_any_vec(c);
        printf("\n");
        printf("  d = ");
        print_any_vec(d);
        printf("\n");

        test_vec("vec3()", ref, (simd_type)ref);

        ref = mathRnd.nextDirection<base_type>();
        target = simd_type(ref.x, ref.y, ref.z);
        test_vec("vec3( random )", ref, target);

        ref = base_type(a, b);
        target = simd_type((simd_type)a, (simd_type)b);
        test_vec("vec3( a, b )", ref, target);

        ref = (base_type)a += (base_type)b;
        target = (simd_type)a += (simd_type)b;
        test_vec("vec3( a += b )", ref, target);

        ref = (base_type)a -= (base_type)b;
        target = (simd_type)a -= (simd_type)b;
        test_vec("vec3( a -= b )", ref, target);

        ref = -a;
        target = -(simd_type)a;
        test_vec("vec3( -a )", ref, target);

        ref = (base_type)a *= (base_type)b;
        target = (simd_type)a *= (simd_type)b;
        test_vec("vec3( a *= b )", ref, target);

        ref = (base_type)a /= (base_type)b;
        target = (simd_type)a /= (simd_type)b;
        test_vec("vec3( a /= b )", ref, target);

        ref = (base_type)a += (float)b.x;
        target = (simd_type)a += (float)b.x;
        test_vec("vec3( a += b.x )", ref, target);

        ref = (base_type)a -= (float)b.x;
        target = (simd_type)a -= (float)b.x;
        test_vec("vec3( a -= b.x )", ref, target);

        ref = (base_type)a *= (float)b.x;
        target = (simd_type)a *= (float)b.x;
        test_vec("vec3( a *= b.x )", ref, target);

        ref = (base_type)a /= (float)b.x;
        target = (simd_type)a /= (float)b.x;
        test_vec("vec3( a /= b.x )", ref, target);

        printf("\n[vec3f op]\n\n");

        ref = OP<base_type>::clamp(a, -0.7f, 0.7f);
        target = OP<simd_type>::clamp((simd_type)a, -0.7f, 0.7f);
        test_vec("clamp( a, -0.7, 0.7 )", ref, target);

        ref = OP<base_type>::dot(a, b);
        target = OP<simd_type>::dot((simd_type)a, (simd_type)b);
        test_vec("dot( a, b )", ref, target);

        ref = OP<base_type>::normalize(a * 5.0f);
        target = OP<simd_type>::normalize((simd_type)a * 5.0f);
        test_vec("normalize( a * 5.0 )", ref, target);

        ref = OP<base_type>::angleBetween(a, b);
        target = OP<simd_type>::angleBetween((simd_type)a, (simd_type)b);
        test_vec("angleBetween( a, b )", ref, target);

        ref = OP<base_type>::cross(a, b);
        target = OP<simd_type>::cross((simd_type)a, (simd_type)b);
        test_vec("cross( a, b )", ref, target);

        ref = OP<base_type>::reflect(a, b);
        target = OP<simd_type>::reflect((simd_type)a, (simd_type)b);
        test_vec("reflect( a, b )", ref, target);

        ref = OP<base_type>::refract(a, b, 0.5, 0.7);
        target = OP<simd_type>::refract((simd_type)a, (simd_type)b, 0.5, 0.7);
        test_vec("refract( a, b, 0.5, 0.7 )", ref, target);

        ref = OP<base_type>::sqrLength(a * 2.0f);
        target = OP<simd_type>::sqrLength((simd_type)a * 2.0f);
        test_vec("sqrLength( a  * 2.0 )", ref, target);

        ref = OP<base_type>::length(a * 2.0f);
        target = OP<simd_type>::length((simd_type)a * 2.0f);
        test_vec("length( a  * 2.0 )", ref, target);

        ref = OP<base_type>::sqrDistance(a, b);
        target = OP<simd_type>::sqrDistance((simd_type)a, (simd_type)b);
        test_vec("sqrDistance( a, b )", ref, target);

        ref = OP<base_type>::distance(a, b);
        target = OP<simd_type>::distance((simd_type)a, (simd_type)b);
        test_vec("distance( a, b )", ref, target);

        ref = OP<base_type>::parallelComponent(a, b);
        target = OP<simd_type>::parallelComponent((simd_type)a, (simd_type)b);
        test_vec("parallelComponent( a, b )", ref, target);

        ref = OP<base_type>::perpendicularComponent(a, b);
        target = OP<simd_type>::perpendicularComponent((simd_type)a, (simd_type)b);
        test_vec("perpendicularComponent( a, b )", ref, target);

        base_type r1, r2;
        OP<base_type>::vecDecomp(a, b, &r1, &r2);

        simd_type r1_s, r2_s;
        OP<simd_type>::vecDecomp(a, b, &r1_s, &r2_s);

        ref = r1 + r2;
        target = r1_s + r2_s;
        test_vec("vecDecomp( a, b, &r1, &r2 )", ref, target);

        ref = OP<base_type>::quadraticClamp(a, b, 0.7f);
        target = OP<simd_type>::quadraticClamp((simd_type)a, (simd_type)b, 0.7f);
        test_vec("quadraticClamp( a, b, 0.7 )", ref, target);

        ref = OP<base_type>::maximum(a);
        target = OP<simd_type>::maximum((simd_type)a);
        test_vec("maximum( a )", ref, target);

        ref = OP<base_type>::maximum(a, b);
        target = OP<simd_type>::maximum((simd_type)a, (simd_type)b);
        test_vec("maximum( a, b )", ref, target);

        ref = OP<base_type>::minimum(a);
        target = OP<simd_type>::minimum((simd_type)a);
        test_vec("minimum( a )", ref, target);

        ref = OP<base_type>::minimum(a, b);
        target = OP<simd_type>::minimum((simd_type)a, (simd_type)b);
        test_vec("minimum( a, b )", ref, target);

        ref = OP<base_type>::abs(a);
        target = OP<simd_type>::abs((simd_type)a);
        test_vec("abs( a )", ref, target);

        ref = OP<base_type>::lerp(a, b, 0.75f);
        target = OP<simd_type>::lerp((simd_type)a, (simd_type)b, 0.75f);
        test_vec("lerp( a, b, 0.75 )", ref, target);

        ref = OP<base_type>::slerp(a, b, 0.75f);
        target = OP<simd_type>::slerp((simd_type)a, (simd_type)b, 0.75f);
        test_vec("slerp( a, b, 0.75 )", ref, target);

        ref = OP<base_type>::barylerp(0.2f, 0.5f, a, b, 0.75f);
        target = OP<simd_type>::barylerp(0.2f, 0.5f, (simd_type)a, (simd_type)b, 0.75f);
        test_vec("barylerp( 0.2, 0.5, a, b, 0.75 )", ref, target);

        ref = OP<base_type>::blerp(a, a + b, a + b + c, a + c, 0.75f, 0.75f);
        target = OP<simd_type>::blerp((simd_type)a, (simd_type)a + (simd_type)b, (simd_type)a + (simd_type)b + (simd_type)c, (simd_type)a + (simd_type)c, 0.75f, 0.75f);
        test_vec("blerp( a, a + b, a + b + c, a + c, 0.75, 0.75 )", ref, target);

        ref = OP<base_type>::splineCatmullRom(a, b, c, d, 0.75f);
        target = OP<simd_type>::splineCatmullRom((simd_type)a, (simd_type)b, (simd_type)c, (simd_type)d, 0.75f);
        test_vec("splineCatmullRom( a, b, c, d, 0.75 )", ref, target);

        ref = OP<base_type>::move(a, b, 0.3f);
        target = OP<simd_type>::move((simd_type)a, (simd_type)b, 0.3f);
        test_vec("move( a, b, 0.3 )", ref, target);

        ref = OP<base_type>::moveSlerp(a, b, 0.3f);
        target = OP<simd_type>::moveSlerp((simd_type)a, (simd_type)b, 0.3f);
        test_vec("moveSlerp( a, b, 0.3 )", ref, target);

        ref = OP<base_type>::sign(a);
        target = OP<simd_type>::sign((simd_type)a);
        test_vec("sign( a )", ref, target);

        ref = OP<base_type>::floor(a * 2.0f);
        target = OP<simd_type>::floor((simd_type)a * 2.0f);
        test_vec("floor( a * 2.0 )", ref, target);

        ref = OP<base_type>::ceil(a * 2.0f);
        target = OP<simd_type>::ceil((simd_type)a * 2.0f);
        test_vec("ceil( a * 2.0 )", ref, target);

        ref = OP<base_type>::round(a * 2.0f);
        target = OP<simd_type>::round((simd_type)a * 2.0f);
        test_vec("round( a * 2.0 )", ref, target);

        ref = OP<base_type>::fmod(a * 2.0f, b);
        target = OP<simd_type>::fmod((simd_type)a * 2.0f, (simd_type)b);
        test_vec("fmod( a * 2.0, b )", ref, target);

        ref = OP<base_type>::step(a * 2.0f, b);
        target = OP<simd_type>::step((simd_type)a * 2.0f, (simd_type)b);
        test_vec("step( a * 2.0, b )", ref, target);

        ref = OP<base_type>::smoothstep(a, b, c);
        target = OP<simd_type>::smoothstep((simd_type)a, (simd_type)b, (simd_type)c);
        test_vec("smoothstep( a, b, c )", ref, target);
    }

    {
        using base_type = vec4<float, SIMD_TYPE::NONE>;
        using simd_type = vec4f;
        base_type ref;
        simd_type target;

        MathRandomExt<Random> mathRnd(Random32::Instance());

        printf("\n[vec4f internal]\n\n");

        const base_type a = mathRnd.nextDirection<base_type>();
        const base_type b = mathRnd.nextDirection<base_type>();
        const base_type c = mathRnd.nextDirection<base_type>();
        const base_type d = mathRnd.nextDirection<base_type>();
        printf("  a = ");
        print_any_vec(a);
        printf("\n");
        printf("  b = ");
        print_any_vec(b);
        printf("\n");
        printf("  c = ");
        print_any_vec(c);
        printf("\n");
        printf("  d = ");
        print_any_vec(d);
        printf("\n");

        test_vec("vec4()", ref, (simd_type)ref);

        ref = mathRnd.nextDirection<base_type>();
        target = simd_type(ref.x, ref.y, ref.z, ref.w);
        test_vec("vec4( random )", ref, target);

        ref = base_type(a, b);
        target = simd_type((simd_type)a, (simd_type)b);
        test_vec("vec4( a, b )", ref, target);

        ref = (base_type)a += (base_type)b;
        target = (simd_type)a += (simd_type)b;
        test_vec("vec4( a += b )", ref, target);

        ref = (base_type)a -= (base_type)b;
        target = (simd_type)a -= (simd_type)b;
        test_vec("vec4( a -= b )", ref, target);

        ref = -a;
        target = -(simd_type)a;
        test_vec("vec4( -a )", ref, target);

        ref = (base_type)a *= (base_type)b;
        target = (simd_type)a *= (simd_type)b;
        test_vec("vec4( a *= b )", ref, target);

        ref = (base_type)a /= (base_type)b;
        target = (simd_type)a /= (simd_type)b;
        test_vec("vec4( a /= b )", ref, target);

        ref = (base_type)a += (float)b.x;
        target = (simd_type)a += (float)b.x;
        test_vec("vec4( a += b.x )", ref, target);

        ref = (base_type)a -= (float)b.x;
        target = (simd_type)a -= (float)b.x;
        test_vec("vec4( a -= b.x )", ref, target);

        ref = (base_type)a *= (float)b.x;
        target = (simd_type)a *= (float)b.x;
        test_vec("vec4( a *= b.x )", ref, target);

        ref = (base_type)a /= (float)b.x;
        target = (simd_type)a /= (float)b.x;
        test_vec("vec4( a /= b.x )", ref, target);

        printf("\n[vec4f op]\n\n");

        ref = OP<base_type>::clamp(a, -0.7f, 0.7f);
        target = OP<simd_type>::clamp((simd_type)a, -0.7f, 0.7f);
        test_vec("clamp( a, -0.7, 0.7 )", ref, target);

        ref = OP<base_type>::dot(a, b);
        target = OP<simd_type>::dot((simd_type)a, (simd_type)b);
        test_vec("dot( a, b )", ref, target);

        ref = OP<base_type>::normalize(a * 5.0f);
        target = OP<simd_type>::normalize((simd_type)a * 5.0f);
        test_vec("normalize( a * 5.0 )", ref, target);

        ref = OP<base_type>::angleBetween(a, b);
        target = OP<simd_type>::angleBetween((simd_type)a, (simd_type)b);
        test_vec("angleBetween( a, b )", ref, target);

        ref = OP<base_type>::cross(a, b);
        target = OP<simd_type>::cross((simd_type)a, (simd_type)b);
        test_vec("cross( a, b )", ref, target);

        ref = OP<base_type>::reflect(a, b);
        target = OP<simd_type>::reflect((simd_type)a, (simd_type)b);
        test_vec("reflect( a, b )", ref, target);

        ref = OP<base_type>::refract(a, b, 0.5, 0.7);
        target = OP<simd_type>::refract((simd_type)a, (simd_type)b, 0.5, 0.7);
        test_vec("refract( a, b, 0.5, 0.7 )", ref, target);

        ref = OP<base_type>::sqrLength(a * 2.0f);
        target = OP<simd_type>::sqrLength((simd_type)a * 2.0f);
        test_vec("sqrLength( a  * 2.0 )", ref, target);

        ref = OP<base_type>::length(a * 2.0f);
        target = OP<simd_type>::length((simd_type)a * 2.0f);
        test_vec("length( a  * 2.0 )", ref, target);

        ref = OP<base_type>::sqrDistance(a, b);
        target = OP<simd_type>::sqrDistance((simd_type)a, (simd_type)b);
        test_vec("sqrDistance( a, b )", ref, target);

        ref = OP<base_type>::distance(a, b);
        target = OP<simd_type>::distance((simd_type)a, (simd_type)b);
        test_vec("distance( a, b )", ref, target);

        ref = OP<base_type>::parallelComponent(a, b);
        target = OP<simd_type>::parallelComponent((simd_type)a, (simd_type)b);
        test_vec("parallelComponent( a, b )", ref, target);

        ref = OP<base_type>::perpendicularComponent(a, b);
        target = OP<simd_type>::perpendicularComponent((simd_type)a, (simd_type)b);
        test_vec("perpendicularComponent( a, b )", ref, target);

        base_type r1, r2;
        OP<base_type>::vecDecomp(a, b, &r1, &r2);

        simd_type r1_s, r2_s;
        OP<simd_type>::vecDecomp(a, b, &r1_s, &r2_s);

        ref = r1 + r2;
        target = r1_s + r2_s;
        test_vec("vecDecomp( a, b, &r1, &r2 )", ref, target);

        ref = OP<base_type>::quadraticClamp(a, b, 0.7f);
        target = OP<simd_type>::quadraticClamp((simd_type)a, (simd_type)b, 0.7f);
        test_vec("quadraticClamp( a, b, 0.7 )", ref, target);

        ref = OP<base_type>::maximum(a);
        target = OP<simd_type>::maximum((simd_type)a);
        test_vec("maximum( a )", ref, target);

        ref = OP<base_type>::maximum(a, b);
        target = OP<simd_type>::maximum((simd_type)a, (simd_type)b);
        test_vec("maximum( a, b )", ref, target);

        ref = OP<base_type>::minimum(a);
        target = OP<simd_type>::minimum((simd_type)a);
        test_vec("minimum( a )", ref, target);

        ref = OP<base_type>::minimum(a, b);
        target = OP<simd_type>::minimum((simd_type)a, (simd_type)b);
        test_vec("minimum( a, b )", ref, target);

        ref = OP<base_type>::abs(a);
        target = OP<simd_type>::abs((simd_type)a);
        test_vec("abs( a )", ref, target);

        ref = OP<base_type>::lerp(a, b, 0.75f);
        target = OP<simd_type>::lerp((simd_type)a, (simd_type)b, 0.75f);
        test_vec("lerp( a, b, 0.75 )", ref, target);

        ref = OP<base_type>::slerp(a, b, 0.75f);
        target = OP<simd_type>::slerp((simd_type)a, (simd_type)b, 0.75f);
        test_vec("slerp( a, b, 0.75 )", ref, target);

        ref = OP<base_type>::barylerp(0.2f, 0.5f, a, b, 0.75f);
        target = OP<simd_type>::barylerp(0.2f, 0.5f, (simd_type)a, (simd_type)b, 0.75f);
        test_vec("barylerp( 0.2, 0.5, a, b, 0.75 )", ref, target);

        ref = OP<base_type>::blerp(a, a + b, a + b + c, a + c, 0.75f, 0.75f);
        target = OP<simd_type>::blerp((simd_type)a, (simd_type)a + (simd_type)b, (simd_type)a + (simd_type)b + (simd_type)c, (simd_type)a + (simd_type)c, 0.75f, 0.75f);
        test_vec("blerp( a, a + b, a + b + c, a + c, 0.75, 0.75 )", ref, target);

        ref = OP<base_type>::splineCatmullRom(a, b, c, d, 0.75f);
        target = OP<simd_type>::splineCatmullRom((simd_type)a, (simd_type)b, (simd_type)c, (simd_type)d, 0.75f);
        test_vec("splineCatmullRom( a, b, c, d, 0.75 )", ref, target);

        ref = OP<base_type>::move(a, b, 0.3f);
        target = OP<simd_type>::move((simd_type)a, (simd_type)b, 0.3f);
        test_vec("move( a, b, 0.3 )", ref, target);

        ref = OP<base_type>::moveSlerp(a, b, 0.3f);
        target = OP<simd_type>::moveSlerp((simd_type)a, (simd_type)b, 0.3f);
        test_vec("moveSlerp( a, b, 0.3 )", ref, target);

        ref = OP<base_type>::sign(a);
        target = OP<simd_type>::sign((simd_type)a);
        test_vec("sign( a )", ref, target);

        ref = OP<base_type>::floor(a * 2.0f);
        target = OP<simd_type>::floor((simd_type)a * 2.0f);
        test_vec("floor( a * 2.0 )", ref, target);

        ref = OP<base_type>::ceil(a * 2.0f);
        target = OP<simd_type>::ceil((simd_type)a * 2.0f);
        test_vec("ceil( a * 2.0 )", ref, target);

        ref = OP<base_type>::round(a * 2.0f);
        target = OP<simd_type>::round((simd_type)a * 2.0f);
        test_vec("round( a * 2.0 )", ref, target);

        ref = OP<base_type>::fmod(a * 2.0f, b);
        target = OP<simd_type>::fmod((simd_type)a * 2.0f, (simd_type)b);
        test_vec("fmod( a * 2.0, b )", ref, target);

        ref = OP<base_type>::step(a * 2.0f, b);
        target = OP<simd_type>::step((simd_type)a * 2.0f, (simd_type)b);
        test_vec("step( a * 2.0, b )", ref, target);

        ref = OP<base_type>::smoothstep(a, b, c);
        target = OP<simd_type>::smoothstep((simd_type)a, (simd_type)b, (simd_type)c);
        test_vec("smoothstep( a, b, c )", ref, target);
    }

    {
        using base_type = quat<float, SIMD_TYPE::NONE>;
        using simd_type = quatf;
        base_type ref;
        simd_type target;

        MathRandomExt<Random> mathRnd(Random32::Instance());

        printf("\n[quatf internal]\n\n");

        base_type a = mathRnd.next<base_type>() *= 2.0f;
        base_type b = mathRnd.next<base_type>() *= 2.0f;
        base_type c = mathRnd.next<base_type>() *= 2.0f;
        base_type d = mathRnd.next<base_type>() *= 2.0f;

        for (auto &v : a.array)
            v -= 1.0f;
        for (auto &v : b.array)
            v -= 1.0f;
        for (auto &v : c.array)
            v -= 1.0f;
        for (auto &v : d.array)
            v -= 1.0f;

        printf("  a = ");
        print_any_vec(a);
        printf("\n");
        printf("  b = ");
        print_any_vec(b);
        printf("\n");
        printf("  c = ");
        print_any_vec(c);
        printf("\n");
        printf("  d = ");
        print_any_vec(d);
        printf("\n");

        test_vec("quat()", ref, (simd_type)ref);

        ref = mathRnd.next<base_type>();
        target = simd_type(ref.x, ref.y, ref.z, ref.w);
        test_vec("quat( random )", ref, target);

        ref = -a;
        target = -(simd_type)a;
        test_vec("quat( -a )", ref, target);

        ref = (base_type)a *= (float)b.x;
        target = (simd_type)a *= (float)b.x;
        test_vec("quat( a *= b.x )", ref, target);

        printf("\n[quatf op]\n\n");

        ref = OP<base_type>::conjugate(a);
        target = OP<simd_type>::conjugate((simd_type)a);
        test_vec("conjugate( a )", ref, target);

        ref = base_type(OP<base_type>::dot(a, b), 0, 0, 0);
        target = simd_type(OP<simd_type>::dot((simd_type)a, (simd_type)b), 0, 0, 0);
        test_vec("dot( a, b )", ref, target);

        ref = OP<base_type>::normalize(a);
        target = OP<simd_type>::normalize((simd_type)a);
        test_vec("normalize( a * 5.0 )", ref, target);

        ref = base_type(OP<base_type>::angleBetween(a, b), 0, 0, 0);
        target = simd_type(OP<simd_type>::angleBetween((simd_type)a, (simd_type)b), 0, 0, 0);
        test_vec("angleBetween( a, b )", ref, target);

        ref = base_type(OP<base_type>::sqrLength(a), 0, 0, 0);
        target = simd_type(OP<simd_type>::sqrLength((simd_type)a), 0, 0, 0);
        test_vec("sqrLength( a )", ref, target);

        ref = base_type(OP<base_type>::length(a), 0, 0, 0);
        target = simd_type(OP<simd_type>::length((simd_type)a), 0, 0, 0);
        test_vec("length( a )", ref, target);

        ref = OP<base_type>::slerp(a, b, 0.75f);
        target = OP<simd_type>::slerp((simd_type)a, (simd_type)b, 0.75f);
        test_vec("slerp( a, b, 0.75 )", ref, target);

        vec3<float, SIMD_TYPE::NONE> axis;
        float angle;
        OP<base_type>::extractAxisAngle(a, &axis, &angle);
        ref = base_type(axis.x, axis.y, axis.z, angle);

        vec3f axis_simd;
        OP<simd_type>::extractAxisAngle(a, &axis_simd, &angle);
        target = simd_type(axis.x, axis.y, axis.z, angle);

        test_vec("extractAxisAngle( a, &axis, &angle )", ref, target);

        float row, pitch, yaw;
        OP<base_type>::extractEuler(a, &row, &pitch, &yaw);
        ref = base_type(row, pitch, yaw, 0);

        OP<simd_type>::extractEuler(a, &row, &pitch, &yaw);
        target = simd_type(row, pitch, yaw, 0);

        test_vec("extractEuler( a, &row, &pitch, &yaw )", ref, target);

        ref = OP<base_type>::inverse(a);
        target = OP<simd_type>::inverse((simd_type)a);
        test_vec("inverse( a )", ref, target);

        ref = OP<base_type>::moveSlerp(a, b, 0.3f);
        target = OP<simd_type>::moveSlerp((simd_type)a, (simd_type)b, 0.3f);
        test_vec("moveSlerp( a, b, 0.3 )", ref, target);

        printf("\n[quatf gen]\n\n");

        const vec4<float, SIMD_TYPE::NONE> av4 = mathRnd.nextDirection<vec4<float, SIMD_TYPE::NONE>>();
        const vec4<float, SIMD_TYPE::NONE> av4_b = mathRnd.nextDirection<vec4<float, SIMD_TYPE::NONE>>();
        const vec3<float, SIMD_TYPE::NONE> av3 = mathRnd.nextDirection<vec3<float, SIMD_TYPE::NONE>>();
        const vec3<float, SIMD_TYPE::NONE> av3_b = mathRnd.nextDirection<vec3<float, SIMD_TYPE::NONE>>();
        const vec2<float, SIMD_TYPE::NONE> av2 = mathRnd.nextDirection<vec2<float, SIMD_TYPE::NONE>>();

        const vec4f av4_simd = av4;
        const vec4f av4_b_simd = av4_b;
        const vec3f av3_simd = av3;
        const vec3f av3_b_simd = av3_b;
        const vec2f av2_simd = av2;

        angle = mathRnd.next01<float>();
        const float angle2 = mathRnd.next01<float>();
        const float angle3 = mathRnd.next01<float>();

        // ref = GEN<base_type>::fromAxis(av4);
        // target = GEN<simd_type>::fromAxis(av4_simd);
        // test_vec("fromAxis( vec4 )", ref, target);

        // ref = GEN<base_type>::fromAxis(av3);
        // target = GEN<simd_type>::fromAxis(av3_simd);
        // test_vec("fromAxis( vec3 )", ref, target);

        // ref = GEN<base_type>::fromAxis(av2);
        // target = GEN<simd_type>::fromAxis(av2_simd);
        // test_vec("fromAxis( vec2 )", ref, target);

        ref = GEN<base_type>::fromAxisAngle(av4, angle);
        target = GEN<simd_type>::fromAxisAngle(av4_simd, angle);
        test_vec("fromAxisAngle( vec4, angle )", ref, target);

        ref = GEN<base_type>::fromAxisAngle(av3, angle);
        target = GEN<simd_type>::fromAxisAngle(av3_simd, angle);
        test_vec("fromAxisAngle( vec3, angle )", ref, target);

        ref = GEN<base_type>::fromAxisAngle(av2, angle);
        target = GEN<simd_type>::fromAxisAngle(av2_simd, angle);
        test_vec("fromAxisAngle( vec2, angle )", ref, target);

        ref = GEN<base_type>::fromEuler(angle, angle2, angle3);
        target = GEN<simd_type>::fromEuler(angle, angle2, angle3);
        test_vec("fromEuler( angle, angle2, angle3 )", ref, target);

        ref = GEN<base_type>::lookAtRotationRH(av4, av4_b);
        target = GEN<simd_type>::lookAtRotationRH(av4_simd, av4_b_simd);
        test_vec("lookAtRotationRH( vec4, vec4 )", ref, target);

        ref = GEN<base_type>::lookAtRotationRH(av3, av3_b);
        target = GEN<simd_type>::lookAtRotationRH(av3_simd, av3_b_simd);
        test_vec("lookAtRotationRH( vec3, vec3 )", ref, target);

        ref = GEN<base_type>::lookAtRotationLH(av4, av4_b);
        target = GEN<simd_type>::lookAtRotationLH(av4_simd, av4_b_simd);
        test_vec("lookAtRotationLH( vec4, vec4 )", ref, target);

        ref = GEN<base_type>::lookAtRotationLH(av3, av3_b);
        target = GEN<simd_type>::lookAtRotationLH(av3_simd, av3_b_simd);
        test_vec("lookAtRotationLH( vec3, vec3 )", ref, target);

        const mat3<float, SIMD_TYPE::NONE> _mat3 = (mathRnd.next<mat3<float, SIMD_TYPE::NONE>>() - 0.5f) * 2.0f;
        const mat4<float, SIMD_TYPE::NONE> _mat4 = (mathRnd.next<mat4<float, SIMD_TYPE::NONE>>() - 0.5f) * 2.0f;

        const mat3f _mat3_simd = _mat3;
        const mat4f _mat4_simd = _mat4;

        ref = GEN<base_type>::fromMat4(_mat4);
        target = GEN<simd_type>::fromMat4(_mat4_simd);
        test_vec("fromMat4( mat4 )", ref, target);
    }

    {
        using base_type = mat2<float, SIMD_TYPE::NONE>;
        using simd_type = mat2f;
        base_type ref;
        simd_type target;

        MathRandomExt<Random> mathRnd(Random32::Instance());

        printf("\n[mat2f internal]\n\n");

        const base_type a = mathRnd.next<base_type>() * 2.0 - 1.0;
        const base_type b = mathRnd.next<base_type>() * 2.0 - 1.0;
        const base_type c = mathRnd.next<base_type>() * 2.0 - 1.0;
        const base_type d = mathRnd.next<base_type>() * 2.0 - 1.0;
        printf("  a = ");
        print_any_vec(a);
        printf("\n");
        printf("  b = ");
        print_any_vec(b);
        printf("\n");
        printf("  c = ");
        print_any_vec(c);
        printf("\n");
        printf("  d = ");
        print_any_vec(d);
        printf("\n");

        test_vec("mat2()", ref, (simd_type)ref);

        ref = mathRnd.next<base_type>() * 2.0 - 1.0;
        target = simd_type(
            ref.a1, ref.b1,
            ref.a2, ref.b2);
        test_vec("mat2( random )", ref, target);

        ref = base_type(0.5f);
        target = simd_type(0.5f);
        test_vec("mat2( 0.5f )", ref, target);

        ref = (base_type)a += (base_type)b;
        target = (simd_type)a += (simd_type)b;
        test_vec("mat2( a += b )", ref, target);

        ref = (base_type)a -= (base_type)b;
        target = (simd_type)a -= (simd_type)b;
        test_vec("mat2( a -= b )", ref, target);

        ref = -a;
        target = -(simd_type)a;
        test_vec("mat2( -a )", ref, target);

        ref = (base_type)a *= (base_type)b;
        target = (simd_type)a *= (simd_type)b;
        test_vec("mat2( a *= b )", ref, target);

        ref = (base_type)a /= (base_type)b;
        target = (simd_type)a /= (simd_type)b;
        test_vec("mat2( a /= b )", ref, target);

        ref = (base_type)a += (float)b.a1;
        target = (simd_type)a += (float)b.a1;
        test_vec("mat2( a += b.a1 )", ref, target);

        ref = (base_type)a -= (float)b.a1;
        target = (simd_type)a -= (float)b.a1;
        test_vec("mat2( a -= b.a1 )", ref, target);

        ref = (base_type)a *= (float)b.a1;
        target = (simd_type)a *= (float)b.a1;
        test_vec("mat2( a *= b.a1 )", ref, target);

        ref = (base_type)a /= (float)b.a1;
        target = (simd_type)a /= (float)b.a1;
        test_vec("mat2( a /= b.a1 )", ref, target);

        ref = ((base_type)a).inverse();
        target = ((simd_type)a).inverse();
        test_vec("mat2().inverse()", ref, target);

        printf("\n[mat2f op]\n\n");

        ref = OP<base_type>::clamp(a, -0.7f, 0.7f);
        target = OP<simd_type>::clamp((simd_type)a, -0.7f, 0.7f);
        test_vec("clamp( a, -0.7, 0.7 )", ref, target);

        ref = OP<base_type>::dot(a, b);
        target = OP<simd_type>::dot((simd_type)a, (simd_type)b);
        test_vec("dot( a, b )", ref, target);

        ref = OP<base_type>::normalize(a * 5.0f);
        target = OP<simd_type>::normalize((simd_type)a * 5.0f);
        test_vec("normalize( a * 5.0 )", ref, target);

        ref = OP<base_type>::sqrLength(a * 2.0f);
        target = OP<simd_type>::sqrLength((simd_type)a * 2.0f);
        test_vec("sqrLength( a  * 2.0 )", ref, target);

        ref = OP<base_type>::length(a * 2.0f);
        target = OP<simd_type>::length((simd_type)a * 2.0f);
        test_vec("length( a  * 2.0 )", ref, target);

        ref = OP<base_type>::sqrDistance(a, b);
        target = OP<simd_type>::sqrDistance((simd_type)a, (simd_type)b);
        test_vec("sqrDistance( a, b )", ref, target);

        ref = OP<base_type>::distance(a, b);
        target = OP<simd_type>::distance((simd_type)a, (simd_type)b);
        test_vec("distance( a, b )", ref, target);

        ref = OP<base_type>::maximum(a);
        target = OP<simd_type>::maximum((simd_type)a);
        test_vec("maximum( a )", ref, target);

        ref = OP<base_type>::maximum(a, b);
        target = OP<simd_type>::maximum((simd_type)a, (simd_type)b);
        test_vec("maximum( a, b )", ref, target);

        ref = OP<base_type>::minimum(a);
        target = OP<simd_type>::minimum((simd_type)a);
        test_vec("minimum( a )", ref, target);

        ref = OP<base_type>::minimum(a, b);
        target = OP<simd_type>::minimum((simd_type)a, (simd_type)b);
        test_vec("minimum( a, b )", ref, target);

        ref = OP<base_type>::abs(a);
        target = OP<simd_type>::abs((simd_type)a);
        test_vec("abs( a )", ref, target);

        ref = OP<base_type>::lerp(a, b, 0.75f);
        target = OP<simd_type>::lerp((simd_type)a, (simd_type)b, 0.75f);
        test_vec("lerp( a, b, 0.75 )", ref, target);

        ref = OP<base_type>::barylerp(0.2f, 0.5f, a, b, 0.75f);
        target = OP<simd_type>::barylerp(0.2f, 0.5f, (simd_type)a, (simd_type)b, 0.75f);
        test_vec("barylerp( 0.2, 0.5, a, b, 0.75 )", ref, target);

        ref = OP<base_type>::blerp(a, a + b, a + b + c, a + c, 0.75f, 0.75f);
        target = OP<simd_type>::blerp((simd_type)a, (simd_type)a + (simd_type)b, (simd_type)a + (simd_type)b + (simd_type)c, (simd_type)a + (simd_type)c, 0.75f, 0.75f);
        test_vec("blerp( a, a + b, a + b + c, a + c, 0.75, 0.75 )", ref, target);

        ref = base_type(OP<base_type>::extractXaxis(a), vec2<float, SIMD_TYPE::NONE>());
        target = simd_type(OP<simd_type>::extractXaxis((simd_type)a), vec2f());
        test_vec("extractXaxis( a )", ref, target);

        ref = base_type(OP<base_type>::extractYaxis(a), vec2<float, SIMD_TYPE::NONE>());
        target = simd_type(OP<simd_type>::extractYaxis((simd_type)a), vec2f());
        test_vec("extractYaxis( a )", ref, target);

        ref = OP<base_type>::transpose(a);
        target = OP<simd_type>::transpose((simd_type)a);
        test_vec("transpose( a )", ref, target);

        ref = OP<base_type>::determinant(a);
        target = OP<simd_type>::determinant((simd_type)a);
        test_vec("determinant( a )", ref, target);

        ref = OP<base_type>::move(a, b, 0.3f);
        target = OP<simd_type>::move((simd_type)a, (simd_type)b, 0.3f);
        test_vec("move( a, b, 0.3 )", ref, target);

        ref = OP<base_type>::sign(a);
        target = OP<simd_type>::sign((simd_type)a);
        test_vec("sign( a )", ref, target);

        ref = OP<base_type>::floor(a * 2.0f);
        target = OP<simd_type>::floor((simd_type)a * 2.0f);
        test_vec("floor( a * 2.0 )", ref, target);

        ref = OP<base_type>::ceil(a * 2.0f);
        target = OP<simd_type>::ceil((simd_type)a * 2.0f);
        test_vec("ceil( a * 2.0 )", ref, target);

        ref = OP<base_type>::round(a * 2.0f);
        target = OP<simd_type>::round((simd_type)a * 2.0f);
        test_vec("round( a * 2.0 )", ref, target);

        ref = OP<base_type>::fmod(a * 2.0f, b);
        target = OP<simd_type>::fmod((simd_type)a * 2.0f, (simd_type)b);
        test_vec("fmod( a * 2.0, b )", ref, target);

        ref = OP<base_type>::step(a * 2.0f, b);
        target = OP<simd_type>::step((simd_type)a * 2.0f, (simd_type)b);
        test_vec("step( a * 2.0, b )", ref, target);

        ref = OP<base_type>::smoothstep(a, b, c);
        target = OP<simd_type>::smoothstep((simd_type)a, (simd_type)b, (simd_type)c);
        test_vec("smoothstep( a, b, c )", ref, target);
    }

    {
        using base_type = mat3<float, SIMD_TYPE::NONE>;
        using simd_type = mat3f;
        base_type ref;
        simd_type target;

        MathRandomExt<Random> mathRnd(Random32::Instance());

        printf("\n[mat3f internal]\n\n");

        const base_type a = mathRnd.next<base_type>() * 2.0 - 1.0;
        const base_type b = mathRnd.next<base_type>() * 2.0 - 1.0;
        const base_type c = mathRnd.next<base_type>() * 2.0 - 1.0;
        const base_type d = mathRnd.next<base_type>() * 2.0 - 1.0;
        printf("  a = ");
        print_any_vec(a);
        printf("\n");
        printf("  b = ");
        print_any_vec(b);
        printf("\n");
        printf("  c = ");
        print_any_vec(c);
        printf("\n");
        printf("  d = ");
        print_any_vec(d);
        printf("\n");

        test_vec("mat3()", ref, (simd_type)ref);

        ref = mathRnd.next<base_type>() * 2.0 - 1.0;
        target = simd_type(
            ref.a1, ref.b1, ref.c1,
            ref.a2, ref.b2, ref.c2,
            ref.a3, ref.b3, ref.c3);
        test_vec("mat3( random )", ref, target);

        ref = base_type(0.5f);
        target = simd_type(0.5f);
        test_vec("mat3( 0.5f )", ref, target);

        ref = (base_type)a += (base_type)b;
        target = (simd_type)a += (simd_type)b;
        test_vec("mat3( a += b )", ref, target);

        ref = (base_type)a -= (base_type)b;
        target = (simd_type)a -= (simd_type)b;
        test_vec("mat3( a -= b )", ref, target);

        ref = -a;
        target = -(simd_type)a;
        test_vec("mat3( -a )", ref, target);

        ref = (base_type)a *= (base_type)b;
        target = (simd_type)a *= (simd_type)b;
        test_vec("mat3( a *= b )", ref, target);

        ref = (base_type)a /= (base_type)b;
        target = (simd_type)a /= (simd_type)b;
        test_vec("mat3( a /= b )", ref, target);

        ref = (base_type)a += (float)b.a1;
        target = (simd_type)a += (float)b.a1;
        test_vec("mat3( a += b.a1 )", ref, target);

        ref = (base_type)a -= (float)b.a1;
        target = (simd_type)a -= (float)b.a1;
        test_vec("mat3( a -= b.a1 )", ref, target);

        ref = (base_type)a *= (float)b.a1;
        target = (simd_type)a *= (float)b.a1;
        test_vec("mat3( a *= b.a1 )", ref, target);

        ref = (base_type)a /= (float)b.a1;
        target = (simd_type)a /= (float)b.a1;
        test_vec("mat3( a /= b.a1 )", ref, target);

        ref = ((base_type)a).inverse();
        target = ((simd_type)a).inverse();
        test_vec("mat3().inverse()", ref, target);

        ref = ((base_type)a).inverse_transpose_2x2();
        target = ((simd_type)a).inverse_transpose_2x2();
        test_vec("mat3().inverse_transpose_2x2()", ref, target);

        printf("\n[mat3f op]\n\n");

        ref = OP<base_type>::clamp(a, -0.7f, 0.7f);
        target = OP<simd_type>::clamp((simd_type)a, -0.7f, 0.7f);
        test_vec("clamp( a, -0.7, 0.7 )", ref, target);

        ref = OP<base_type>::dot(a, b);
        target = OP<simd_type>::dot((simd_type)a, (simd_type)b);
        test_vec("dot( a, b )", ref, target);

        ref = OP<base_type>::normalize(a * 5.0f);
        target = OP<simd_type>::normalize((simd_type)a * 5.0f);
        test_vec("normalize( a * 5.0 )", ref, target);

        ref = OP<base_type>::sqrLength(a * 2.0f);
        target = OP<simd_type>::sqrLength((simd_type)a * 2.0f);
        test_vec("sqrLength( a  * 2.0 )", ref, target);

        ref = OP<base_type>::length(a * 2.0f);
        target = OP<simd_type>::length((simd_type)a * 2.0f);
        test_vec("length( a  * 2.0 )", ref, target);

        ref = OP<base_type>::sqrDistance(a, b);
        target = OP<simd_type>::sqrDistance((simd_type)a, (simd_type)b);
        test_vec("sqrDistance( a, b )", ref, target);

        ref = OP<base_type>::distance(a, b);
        target = OP<simd_type>::distance((simd_type)a, (simd_type)b);
        test_vec("distance( a, b )", ref, target);

        ref = OP<base_type>::maximum(a);
        target = OP<simd_type>::maximum((simd_type)a);
        test_vec("maximum( a )", ref, target);

        ref = OP<base_type>::maximum(a, b);
        target = OP<simd_type>::maximum((simd_type)a, (simd_type)b);
        test_vec("maximum( a, b )", ref, target);

        ref = OP<base_type>::minimum(a);
        target = OP<simd_type>::minimum((simd_type)a);
        test_vec("minimum( a )", ref, target);

        ref = OP<base_type>::minimum(a, b);
        target = OP<simd_type>::minimum((simd_type)a, (simd_type)b);
        test_vec("minimum( a, b )", ref, target);

        ref = OP<base_type>::abs(a);
        target = OP<simd_type>::abs((simd_type)a);
        test_vec("abs( a )", ref, target);

        ref = OP<base_type>::lerp(a, b, 0.75f);
        target = OP<simd_type>::lerp((simd_type)a, (simd_type)b, 0.75f);
        test_vec("lerp( a, b, 0.75 )", ref, target);

        ref = OP<base_type>::barylerp(0.2f, 0.5f, a, b, 0.75f);
        target = OP<simd_type>::barylerp(0.2f, 0.5f, (simd_type)a, (simd_type)b, 0.75f);
        test_vec("barylerp( 0.2, 0.5, a, b, 0.75 )", ref, target);

        ref = OP<base_type>::blerp(a, a + b, a + b + c, a + c, 0.75f, 0.75f);
        target = OP<simd_type>::blerp((simd_type)a, (simd_type)a + (simd_type)b, (simd_type)a + (simd_type)b + (simd_type)c, (simd_type)a + (simd_type)c, 0.75f, 0.75f);
        test_vec("blerp( a, a + b, a + b + c, a + c, 0.75, 0.75 )", ref, target);

        ref = OP<base_type>::extractRotation(a);
        target = OP<simd_type>::extractRotation((simd_type)a);
        test_vec("extractRotation( a )", ref, target);

        ref = base_type(OP<base_type>::extractXaxis(a), 0, 0);
        target = simd_type(OP<simd_type>::extractXaxis((simd_type)a), 0, 0);
        test_vec("extractXaxis( a )", ref, target);

        ref = base_type(OP<base_type>::extractYaxis(a), 0, 0);
        target = simd_type(OP<simd_type>::extractYaxis((simd_type)a), 0, 0);
        test_vec("extractYaxis( a )", ref, target);

        ref = base_type(OP<base_type>::extractZaxis(a), 0, 0);
        target = simd_type(OP<simd_type>::extractZaxis((simd_type)a), 0, 0);
        test_vec("extractZaxis( a )", ref, target);

        ref = base_type(OP<base_type>::extractTranslation(a), 0, 0);
        target = simd_type(OP<simd_type>::extractTranslation((simd_type)a), 0, 0);
        test_vec("extractTranslation( a )", ref, target);

        ref = OP<base_type>::transpose(a);
        target = OP<simd_type>::transpose((simd_type)a);
        test_vec("transpose( a )", ref, target);

        ref = OP<base_type>::determinant(a);
        target = OP<simd_type>::determinant((simd_type)a);
        test_vec("determinant( a )", ref, target);

        float row, pitch, yaw;
        OP<base_type>::extractEuler(a, &row, &pitch, &yaw);
        ref = base_type(vec3f(row, pitch, yaw), 0, 0);

        OP<simd_type>::extractEuler((simd_type)a, &row, &pitch, &yaw);
        target = simd_type(vec3f(row, pitch, yaw), 0, 0);

        test_vec("extractEuler( a, &row, &pitch, &yaw )", ref, target);

        ref = OP<base_type>::move(a, b, 0.3f);
        target = OP<simd_type>::move((simd_type)a, (simd_type)b, 0.3f);
        test_vec("move( a, b, 0.3 )", ref, target);

        ref = OP<base_type>::sign(a);
        target = OP<simd_type>::sign((simd_type)a);
        test_vec("sign( a )", ref, target);

        ref = OP<base_type>::floor(a * 2.0f);
        target = OP<simd_type>::floor((simd_type)a * 2.0f);
        test_vec("floor( a * 2.0 )", ref, target);

        ref = OP<base_type>::ceil(a * 2.0f);
        target = OP<simd_type>::ceil((simd_type)a * 2.0f);
        test_vec("ceil( a * 2.0 )", ref, target);

        ref = OP<base_type>::round(a * 2.0f);
        target = OP<simd_type>::round((simd_type)a * 2.0f);
        test_vec("round( a * 2.0 )", ref, target);

        ref = OP<base_type>::fmod(a * 2.0f, b);
        target = OP<simd_type>::fmod((simd_type)a * 2.0f, (simd_type)b);
        test_vec("fmod( a * 2.0, b )", ref, target);

        ref = OP<base_type>::step(a * 2.0f, b);
        target = OP<simd_type>::step((simd_type)a * 2.0f, (simd_type)b);
        test_vec("step( a * 2.0, b )", ref, target);

        ref = OP<base_type>::smoothstep(a, b, c);
        target = OP<simd_type>::smoothstep((simd_type)a, (simd_type)b, (simd_type)c);
        test_vec("smoothstep( a, b, c )", ref, target);
    }

    {
        using base_type = mat4<float, SIMD_TYPE::NONE>;
        using simd_type = mat4f;
        base_type ref;
        simd_type target;

        MathRandomExt<Random> mathRnd(Random32::Instance());

        printf("\n[mat4f internal]\n\n");

        const base_type a = mathRnd.next<base_type>() * 2.0 - 1.0;
        const base_type b = mathRnd.next<base_type>() * 2.0 - 1.0;
        const base_type c = mathRnd.next<base_type>() * 2.0 - 1.0;
        const base_type d = mathRnd.next<base_type>() * 2.0 - 1.0;
        printf("  a = ");
        print_any_vec(a);
        printf("\n");
        printf("  b = ");
        print_any_vec(b);
        printf("\n");
        printf("  c = ");
        print_any_vec(c);
        printf("\n");
        printf("  d = ");
        print_any_vec(d);
        printf("\n");

        test_vec("mat4()", ref, (simd_type)ref);

        ref = mathRnd.next<base_type>() * 2.0 - 1.0;
        target = simd_type(
            ref.a1, ref.b1, ref.c1, ref.d1,
            ref.a2, ref.b2, ref.c2, ref.d2,
            ref.a3, ref.b3, ref.c3, ref.d3,
            ref.a4, ref.b4, ref.c4, ref.d4);
        test_vec("mat4( random )", ref, target);

        ref = base_type(0.5f);
        target = simd_type(0.5f);
        test_vec("mat4( 0.5f )", ref, target);

        ref = (base_type)a += (base_type)b;
        target = (simd_type)a += (simd_type)b;
        test_vec("mat4( a += b )", ref, target);

        ref = (base_type)a -= (base_type)b;
        target = (simd_type)a -= (simd_type)b;
        test_vec("mat4( a -= b )", ref, target);

        ref = -a;
        target = -(simd_type)a;
        test_vec("mat4( -a )", ref, target);

        ref = (base_type)a *= (base_type)b;
        target = (simd_type)a *= (simd_type)b;
        test_vec("mat4( a *= b )", ref, target);

        ref = (base_type)a /= (base_type)b;
        target = (simd_type)a /= (simd_type)b;
        test_vec("mat4( a /= b )", ref, target);

        ref = (base_type)a += (float)b.a1;
        target = (simd_type)a += (float)b.a1;
        test_vec("mat4( a += b.a1 )", ref, target);

        ref = (base_type)a -= (float)b.a1;
        target = (simd_type)a -= (float)b.a1;
        test_vec("mat4( a -= b.a1 )", ref, target);

        ref = (base_type)a *= (float)b.a1;
        target = (simd_type)a *= (float)b.a1;
        test_vec("mat4( a *= b.a1 )", ref, target);

        ref = (base_type)a /= (float)b.a1;
        target = (simd_type)a /= (float)b.a1;
        test_vec("mat4( a /= b.a1 )", ref, target);

        ref = ((base_type)a).inverse();
        target = ((simd_type)a).inverse();
        test_vec("mat4().inverse()", ref, target);

        ref = ((base_type)a).inverse_transpose_3x3();
        target = ((simd_type)a).inverse_transpose_3x3();
        test_vec("mat4().inverse_transpose_3x3()", ref, target);

        ref = ((base_type)a).inverse_transpose_2x2();
        target = ((simd_type)a).inverse_transpose_2x2();
        test_vec("mat4().inverse_transpose_2x2()", ref, target);

        printf("\n[mat4f op]\n\n");

        ref = OP<base_type>::clamp(a, -0.7f, 0.7f);
        target = OP<simd_type>::clamp((simd_type)a, -0.7f, 0.7f);
        test_vec("clamp( a, -0.7, 0.7 )", ref, target);

        ref = OP<base_type>::dot(a, b);
        target = OP<simd_type>::dot((simd_type)a, (simd_type)b);
        test_vec("dot( a, b )", ref, target);

        ref = OP<base_type>::normalize(a * 5.0f);
        target = OP<simd_type>::normalize((simd_type)a * 5.0f);
        test_vec("normalize( a * 5.0 )", ref, target);

        ref = OP<base_type>::sqrLength(a * 2.0f);
        target = OP<simd_type>::sqrLength((simd_type)a * 2.0f);
        test_vec("sqrLength( a  * 2.0 )", ref, target);

        ref = OP<base_type>::length(a * 2.0f);
        target = OP<simd_type>::length((simd_type)a * 2.0f);
        test_vec("length( a  * 2.0 )", ref, target);

        ref = OP<base_type>::sqrDistance(a, b);
        target = OP<simd_type>::sqrDistance((simd_type)a, (simd_type)b);
        test_vec("sqrDistance( a, b )", ref, target);

        ref = OP<base_type>::distance(a, b);
        target = OP<simd_type>::distance((simd_type)a, (simd_type)b);
        test_vec("distance( a, b )", ref, target);

        ref = OP<base_type>::maximum(a);
        target = OP<simd_type>::maximum((simd_type)a);
        test_vec("maximum( a )", ref, target);

        ref = OP<base_type>::maximum(a, b);
        target = OP<simd_type>::maximum((simd_type)a, (simd_type)b);
        test_vec("maximum( a, b )", ref, target);

        ref = OP<base_type>::minimum(a);
        target = OP<simd_type>::minimum((simd_type)a);
        test_vec("minimum( a )", ref, target);

        ref = OP<base_type>::minimum(a, b);
        target = OP<simd_type>::minimum((simd_type)a, (simd_type)b);
        test_vec("minimum( a, b )", ref, target);

        ref = OP<base_type>::abs(a);
        target = OP<simd_type>::abs((simd_type)a);
        test_vec("abs( a )", ref, target);

        ref = OP<base_type>::lerp(a, b, 0.75f);
        target = OP<simd_type>::lerp((simd_type)a, (simd_type)b, 0.75f);
        test_vec("lerp( a, b, 0.75 )", ref, target);

        ref = OP<base_type>::barylerp(0.2f, 0.5f, a, b, 0.75f);
        target = OP<simd_type>::barylerp(0.2f, 0.5f, (simd_type)a, (simd_type)b, 0.75f);
        test_vec("barylerp( 0.2, 0.5, a, b, 0.75 )", ref, target);

        ref = OP<base_type>::blerp(a, a + b, a + b + c, a + c, 0.75f, 0.75f);
        target = OP<simd_type>::blerp((simd_type)a, (simd_type)a + (simd_type)b, (simd_type)a + (simd_type)b + (simd_type)c, (simd_type)a + (simd_type)c, 0.75f, 0.75f);
        test_vec("blerp( a, a + b, a + b + c, a + c, 0.75, 0.75 )", ref, target);

        ref = OP<base_type>::extractRotation(a);
        target = OP<simd_type>::extractRotation((simd_type)a);
        test_vec("extractRotation( a )", ref, target);

        ref = OP<base_type>::extractRotation_2x2(a);
        target = OP<simd_type>::extractRotation_2x2((simd_type)a);
        test_vec("extractRotation_2x2( a )", ref, target);

        ref = base_type(OP<base_type>::extractXaxis(a), 0, 0, 0);
        target = simd_type(OP<simd_type>::extractXaxis((simd_type)a), 0, 0, 0);
        test_vec("extractXaxis( a )", ref, target);

        ref = base_type(OP<base_type>::extractYaxis(a), 0, 0, 0);
        target = simd_type(OP<simd_type>::extractYaxis((simd_type)a), 0, 0, 0);
        test_vec("extractYaxis( a )", ref, target);

        ref = base_type(OP<base_type>::extractZaxis(a), 0, 0, 0);
        target = simd_type(OP<simd_type>::extractZaxis((simd_type)a), 0, 0, 0);
        test_vec("extractZaxis( a )", ref, target);

        ref = base_type(OP<base_type>::extractTranslation(a), 0, 0, 0);
        target = simd_type(OP<simd_type>::extractTranslation((simd_type)a), 0, 0, 0);
        test_vec("extractTranslation( a )", ref, target);

        ref = OP<base_type>::transpose(a);
        target = OP<simd_type>::transpose((simd_type)a);
        test_vec("transpose( a )", ref, target);

        ref = OP<base_type>::determinant(a);
        target = OP<simd_type>::determinant((simd_type)a);
        test_vec("determinant( a )", ref, target);

        float row, pitch, yaw;
        OP<base_type>::extractEuler(a, &row, &pitch, &yaw);
        ref = base_type(vec4f(row, pitch, yaw, 0), 0, 0, 0);

        OP<simd_type>::extractEuler((simd_type)a, &row, &pitch, &yaw);
        target = simd_type(vec4f(row, pitch, yaw, 0), 0, 0, 0);

        test_vec("extractEuler( a, &row, &pitch, &yaw )", ref, target);

        ref = OP<base_type>::move(a, b, 0.3f);
        target = OP<simd_type>::move((simd_type)a, (simd_type)b, 0.3f);
        test_vec("move( a, b, 0.3 )", ref, target);

        ref = OP<base_type>::sign(a);
        target = OP<simd_type>::sign((simd_type)a);
        test_vec("sign( a )", ref, target);

        ref = OP<base_type>::floor(a * 2.0f);
        target = OP<simd_type>::floor((simd_type)a * 2.0f);
        test_vec("floor( a * 2.0 )", ref, target);

        ref = OP<base_type>::ceil(a * 2.0f);
        target = OP<simd_type>::ceil((simd_type)a * 2.0f);
        test_vec("ceil( a * 2.0 )", ref, target);

        ref = OP<base_type>::round(a * 2.0f);
        target = OP<simd_type>::round((simd_type)a * 2.0f);
        test_vec("round( a * 2.0 )", ref, target);

        ref = OP<base_type>::fmod(a * 2.0f, b);
        target = OP<simd_type>::fmod((simd_type)a * 2.0f, (simd_type)b);
        test_vec("fmod( a * 2.0, b )", ref, target);

        ref = OP<base_type>::step(a * 2.0f, b);
        target = OP<simd_type>::step((simd_type)a * 2.0f, (simd_type)b);
        test_vec("step( a * 2.0, b )", ref, target);

        ref = OP<base_type>::smoothstep(a, b, c);
        target = OP<simd_type>::smoothstep((simd_type)a, (simd_type)b, (simd_type)c);
        test_vec("smoothstep( a, b, c )", ref, target);
    }

    // */

    return 0;
}
