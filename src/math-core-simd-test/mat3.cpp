
#include "mat.h"

#include "common.h"

void test_mat3()
{

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

        printf("\n[mat3f gen]\n\n");

        const float x = (mathRnd.next01<float>() * 2.0f - 1.0f) * CONSTANT<float>::PI * 2.0f;
        const float y = (mathRnd.next01<float>() * 2.0f - 1.0f) * CONSTANT<float>::PI * 2.0f;
        const float z = (mathRnd.next01<float>() * 2.0f - 1.0f) * CONSTANT<float>::PI * 2.0f;
        const float w = (mathRnd.next01<float>() * 2.0f - 1.0f) * CONSTANT<float>::PI * 2.0f;

        ref = GEN<base_type>::translateHomogeneous(x, y);
        target = GEN<simd_type>::translateHomogeneous(x, y);
        test_vec("translateHomogeneous( x, y )", ref, target);

        ref = GEN<base_type>::translateHomogeneous(vec2f(x, y));
        target = GEN<simd_type>::translateHomogeneous(vec2f(x, y));
        test_vec("translateHomogeneous( vec2f(x, y) )", ref, target);

        ref = GEN<base_type>::translateHomogeneous(vec3f(x, y, z));
        target = GEN<simd_type>::translateHomogeneous(vec3f(x, y, z));
        test_vec("translateHomogeneous( vec3f(x, y, z) )", ref, target);

        ref = GEN<base_type>::translateHomogeneous(vec4f(x, y, z, 1));
        target = GEN<simd_type>::translateHomogeneous(vec4f(x, y, z, 1));
        test_vec("translateHomogeneous( vec4f(x, y, z, 1) )", ref, target);

        ref = GEN<base_type>::scaleHomogeneous(x, y);
        target = GEN<simd_type>::scaleHomogeneous(x, y);
        test_vec("scaleHomogeneous( x, y )", ref, target);

        ref = GEN<base_type>::scaleHomogeneous(vec2f(x, y));
        target = GEN<simd_type>::scaleHomogeneous(vec2f(x, y));
        test_vec("scaleHomogeneous( vec2f(x, y) )", ref, target);

        ref = GEN<base_type>::scaleHomogeneous(vec3f(x, y, z));
        target = GEN<simd_type>::scaleHomogeneous(vec3f(x, y, z));
        test_vec("scaleHomogeneous( vec3f(x, y, z) )", ref, target);

        ref = GEN<base_type>::scaleHomogeneous(vec4f(x, y, z, 0));
        target = GEN<simd_type>::scaleHomogeneous(vec4f(x, y, z, 0));
        test_vec("scaleHomogeneous( vec4f(x, y, z, 0) )", ref, target);

        ref = GEN<base_type>::zRotateHomogeneous(x);
        target = GEN<simd_type>::zRotateHomogeneous(x);
        test_vec("zRotateHomogeneous( x )", ref, target);

        ref = GEN<base_type>::scale(x, y, z);
        target = GEN<simd_type>::scale(x, y, z);
        test_vec("scale( x, y, z )", ref, target);

        ref = GEN<base_type>::scale(vec2f(x, y));
        target = GEN<simd_type>::scale(vec2f(x, y));
        test_vec("scale( vec2f(x, y) )", ref, target);

        ref = GEN<base_type>::scale(vec3f(x, y, z));
        target = GEN<simd_type>::scale(vec3f(x, y, z));
        test_vec("scale( vec3f(x, y, z) )", ref, target);

        ref = GEN<base_type>::scale(vec4f(x, y, z, 0));
        target = GEN<simd_type>::scale(vec4f(x, y, z, 0));
        test_vec("scale( vec4f(x, y, z, 0) )", ref, target);


        ref = GEN<base_type>::xRotate(x);
        target = GEN<simd_type>::xRotate(x);
        test_vec("xRotate( x )", ref, target);

        ref = GEN<base_type>::yRotate(x);
        target = GEN<simd_type>::yRotate(x);
        test_vec("yRotate( x )", ref, target);

        ref = GEN<base_type>::zRotate(x);
        target = GEN<simd_type>::zRotate(x);
        test_vec("zRotate( x )", ref, target);


        ref = GEN<base_type>::fromEuler(x, y, z);
        target = GEN<simd_type>::fromEuler(x, y, z);
        test_vec("fromEuler( x, y, z )", ref, target);

        const vec2f axis_v2 = mathRnd.nextDirection<vec2f>();
        const vec3f axis_v3 = mathRnd.nextDirection<vec3f>();
        const vec4f axis_v4 = mathRnd.nextDirection<vec4f>(true);

        ref = GEN<base_type>::rotate(x, axis_v3.x, axis_v3.y, axis_v3.z);
        target = GEN<simd_type>::rotate(x, axis_v3.x, axis_v3.y, axis_v3.z);
        test_vec("rotate( x, axis_v3.x, axis_v3.y, axis_v3.z )", ref, target);

        ref = GEN<base_type>::rotate(x, axis_v2);
        target = GEN<simd_type>::rotate(x, axis_v2);
        test_vec("rotate( x, axis_v2 )", ref, target);

        ref = GEN<base_type>::rotate(x, axis_v3);
        target = GEN<simd_type>::rotate(x, axis_v3);
        test_vec("rotate( x, axis_v3 )", ref, target);

        ref = GEN<base_type>::rotate(x, axis_v4);
        target = GEN<simd_type>::rotate(x, axis_v4);
        test_vec("rotate( x, axis_v4 )", ref, target);

        const vec3f up_v3 = mathRnd.nextDirection<vec3f>();
        const vec4f up_v4 = mathRnd.nextDirection<vec4f>(true);

        const vec3f pos_v3 = vec3f(x, y, z);
        const vec4f pos_v4 = vec4f(x, y, z, 1);

        const vec2f front_v2 = mathRnd.nextDirection<vec2f>();
        const vec2f pos_v2 = vec2f(x, y);

        ref = GEN<base_type>::lookAtRotationRH(front_v2, pos_v2);
        target = GEN<simd_type>::lookAtRotationRH(front_v2, pos_v2);
        test_vec("lookAtRotationRH( front_v2, pos_v2 )", ref, target);

        ref = GEN<base_type>::lookAtRotationLH(front_v2, pos_v2);
        target = GEN<simd_type>::lookAtRotationLH(front_v2, pos_v2);
        test_vec("modelLookAtLH( front_v2, pos_v2 )", ref, target);

        quatf quat = GEN<quatf>::fromEuler(x, y, z);

        ref = GEN<base_type>::fromQuat(quat);
        target = GEN<simd_type>::fromQuat(quat);
        test_vec("fromQuat( quat )", ref, target);

        mat2f mat2 = mathRnd.next<mat2f>() * 2.0f - 1.0f;

        ref = GEN<base_type>::fromMat2(mat2);
        target = GEN<simd_type>::fromMat2(mat2);
        test_vec("fromMat2( mat2 )", ref, target);

        mat4f mat4 = mathRnd.next<mat4f>() * 2.0f - 1.0f;

        ref = GEN<base_type>::fromMat4(mat4);
        target = GEN<simd_type>::fromMat4(mat4);
        test_vec("fromMat4( mat4 )", ref, target);
    }
}
