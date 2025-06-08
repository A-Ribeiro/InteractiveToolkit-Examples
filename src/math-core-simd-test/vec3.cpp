
#include "vec.h"

#include "common.h"

void test_vec3() {

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

        ref = OP<base_type>::orientation(a, b, c, d);
        target = OP<simd_type>::orientation((simd_type)a, (simd_type)b, (simd_type)c, (simd_type)d);
        test_vec("orientation( a, b, c, d )", ref, target);

        ref = OP<base_type>::reflect(a, b);
        target = OP<simd_type>::reflect((simd_type)a, (simd_type)b);
        test_vec("reflect( a, b )", ref, target);

        ref = OP<base_type>::refract(a, b, 0.5f, 0.7f);
        target = OP<simd_type>::refract((simd_type)a, (simd_type)b, 0.5f, 0.7f);
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

}