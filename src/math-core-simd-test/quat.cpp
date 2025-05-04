
#include "vec.h"

#include "common.h"

void test_quat() {

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

        ref = GEN<base_type>::fromMat3(_mat3);
        target = GEN<simd_type>::fromMat3(_mat3_simd);
        test_vec("fromMat3( mat4 )", ref, target);

        ref = GEN<base_type>::fromMat4(_mat4);
        target = GEN<simd_type>::fromMat4(_mat4_simd);
        test_vec("fromMat4( mat4 )", ref, target);
    }


}