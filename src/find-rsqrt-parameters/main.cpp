#include <InteractiveToolkit/InteractiveToolkit.h>
#include <iostream>

using namespace ITKCommon;
using namespace MathCore;

const int solver_iterations = 100;

template <typename Tf, typename Tui>
class CarmackOptimizer
{
public:
    bool force_estimate_above_real_v;

    Tui estimative_var1;
    double iteration_var2;
    double iteration_var3;

    union
    {
        Tf y_float;
        Tui y_uint;
    };

    CarmackOptimizer() {
        force_estimate_above_real_v = false;
    }

    void setVars(Tui estimative_var1, double iteration_var2, double iteration_var3)
    {
        this->estimative_var1 = estimative_var1;
        this->iteration_var2 = iteration_var2;
        this->iteration_var3 = iteration_var3;
    }

    Tf rsqrt(const Tf &x)
    {
        y_float = x;
        y_uint = estimative_var1 - (y_uint >> 1);
        y_float = y_float * ((Tf)iteration_var2 * ((Tf)iteration_var3 - (x * y_float) * y_float)); // 1st iteration, low precision
        return y_float;
    }

    Tf rsqrt_final(const Tf &x, bool _3rd_iteration)
    {
        y_float = x;
        y_uint = estimative_var1 - (y_uint >> 1);
        y_float = y_float * ((Tf)iteration_var2 * ((Tf)iteration_var3 - (x * y_float) * y_float)); // 1st iteration, low precision
        y_float = y_float * ((Tf)0.5 * ((Tf)3.0 - (x * y_float) * y_float));                       // 2nd iteration
        if (_3rd_iteration)
            y_float = y_float * ((Tf)0.5 * ((Tf)3.0 - (x * y_float) * y_float)); // 3rd iteration
        return y_float;
    }

    double error_s(const double &x)
    {
        double v = (double)rsqrt((Tf)x);
        double expected = (double)(1.0 / OP<double>::sqrt(x));

        double rc = v - expected;
        if (force_estimate_above_real_v)
            if (rc > 0)
                rc = 1.0;
        rc = OP<double>::abs(rc);

        return rc;
    }

    double error(const double &x)
    {
        double acc = 0.0;
        for (int i = 0; i < 10000; i++)
        {
            acc += error_s((double)(i + 1) * 1.5);
        }
        acc /= 10000.0;
        return acc;
    }

    void adjustVar2(double _min_ = 0.5, double _max_ = 1.0)
    {
        double _latest_walk_var = 0.5;

        double _min = _min_;
        double _max = _max_;

        for (int i = 0; i < solver_iterations; i++)
        {
            double middle = (_min + _max) * 0.5;
            double _1st = (_min + middle) * 0.5;
            // optmin_float.setVars(0x5F1FFFF9, _1st, 3.0f);
            iteration_var2 = _1st;
            double _1st_err = error(4.0);
            double _2nd = (middle + _max) * 0.5;
            // optmin_float.setVars(0x5F1FFFF9, _2nd, 3.0f);
            iteration_var2 = _2nd;
            double _2nd_err = error(4.0);
            if (_1st_err < _2nd_err)
            {
                // walk to 1st
                _max = middle;
                _latest_walk_var = _1st;
                // if (i == 29)
                // std::cout << "  walking current error: " << _1st_err << " var: " << _latest_walk_var << " _min: " << _min << " _max: " << _max << std::endl;
            }
            else
            {
                // walk to 2nd
                _min = middle;
                _latest_walk_var = _2nd;
                // if (i == 29)
                // std::cout << "  walking current error: " << _2nd_err << " var: " << _latest_walk_var  << " _min: " << _min << " _max: " << _max << std::endl;
            }
        }
    }

    void adjustVar3(double _min_ = 2.0, double _max_ = 3.0)
    {
        double _latest_walk_var = 0.5;

        double _min = _min_;
        double _max = _max_;

        for (int i = 0; i < solver_iterations; i++)
        {
            double middle = (_min + _max) * 0.5;
            double _1st = (_min + middle) * 0.5;
            // optmin_float.setVars(0x5F1FFFF9, _1st, 3.0f);
            iteration_var3 = _1st;
            adjustVar2();
            double _1st_err = error(4.0);
            double _2nd = (middle + _max) * 0.5;
            // optmin_float.setVars(0x5F1FFFF9, _2nd, 3.0f);
            iteration_var3 = _2nd;
            adjustVar2();
            double _2nd_err = error(4.0);
            if (_1st_err < _2nd_err)
            {
                // walk to 1st
                _max = middle;
                _latest_walk_var = _1st;
                // if (i == 29)
                // std::cout << "  walking current error: " << _1st_err << " var: " << _latest_walk_var << " _min: " << _min << " _max: " << _max << std::endl;
            }
            else
            {
                // walk to 2nd
                _min = middle;
                _latest_walk_var = _2nd;
                // if (i == 29)
                // std::cout << "  walking current error: " << _2nd_err << " var: " << _latest_walk_var  << " _min: " << _min << " _max: " << _max << std::endl;
            }
        }
    }
};

#include <iomanip>
int main(int argc, char *argv[])
{
    Path::setWorkingPath(Path::getExecutablePath(argv[0]));

    auto start_time = std::chrono::high_resolution_clock::now();

    double v = 0;
    for (int i = 1; i < 999999999; i++)
    {
        v += OP<double>::rsqrt(i);
    }
    printf("%f\n", v);
    std::cout << "1.0/sqrt(4.0) = " << std::setprecision(16) << OP<double>::rsqrt(4.0) << std::endl;
    std::cout << "1.0/sqrt(256.0) = " << std::setprecision(16) << OP<double>::rsqrt(256.0) << std::endl;
    std::cout << "1.0/sqrt(1024.0) = " << std::setprecision(16) << OP<double>::rsqrt(1024.0) << std::endl;

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

    printf("Simulation time: %f sec\n", ((double)duration.count() / 1000000.0));
    printf("\n");

    // float
    // 0 10111110 00111111111111111111001
    // 0x5F1FFFF9

    // double
    // 0 10111111110 0011111111111111111111111111111111111111111111111001
    // 0 10111111110 0011111111111111111111111111111111111111111111111001
    // 0x5FE3FFFFFFFFFFF9

    {
        std::cout << "FLOAT: " << std::endl;

        CarmackOptimizer<float, uint32_t> optmin_float;
        optmin_float.setVars(0x5F1FFFF9, 0.5, 3.0);
        std::cout << "starting error: " << optmin_float.error(4.0) << std::endl;
        optmin_float.adjustVar3();
        optmin_float.adjustVar2();
        std::cout << " var2: " << std::setprecision(16) << std::defaultfloat << optmin_float.iteration_var2 << std::endl;
        std::cout << " var3: " << std::setprecision(16) << std::defaultfloat << optmin_float.iteration_var3 << std::endl;
        std::cout << " error: " << std::setprecision(16) << std::defaultfloat << optmin_float.error(4.0) << std::endl;
        std::cout << std::setprecision(6) << std::defaultfloat << optmin_float.rsqrt_final(4.0, false) << std::endl;
    }

    {
        std::cout << "DOUBLE: " << std::endl;

        CarmackOptimizer<double, uint64_t> optmin_float;
        // special case for double...
        optmin_float.force_estimate_above_real_v = true;
        optmin_float.setVars(0x5FE3FFFFFFFFFFF9, 0.5, 3.0);
        std::cout << "starting error: " << optmin_float.error(4.0) << std::endl;
        optmin_float.adjustVar3();
        optmin_float.adjustVar2();

        std::cout << " var2: " << std::setprecision(16) << std::defaultfloat << optmin_float.iteration_var2 << std::endl;
        std::cout << " var3: " << std::setprecision(16) << std::defaultfloat << optmin_float.iteration_var3 << std::endl;
        std::cout << " error: " << std::setprecision(16) << std::defaultfloat << optmin_float.error(4.0) << std::endl;
        std::cout << std::setprecision(15) << std::defaultfloat << optmin_float.rsqrt_final(4.0, true) << std::endl;
    }

    return 0;
}
