
#include "mat.h"

#include "common.h"

void test_mat4() {
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
}
