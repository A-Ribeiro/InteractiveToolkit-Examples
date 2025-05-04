#pragma once

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
