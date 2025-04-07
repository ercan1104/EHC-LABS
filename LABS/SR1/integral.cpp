#include <iostream>
#include <cmath>
#include <string>
#include <chrono>

class Solver
{
private:
    double (*m_func)(float);

    void kahan_sum(double func_ret, double &sum, double &corr)
    {
        const double y = func_ret - corr;
        const double t = sum + y;
        corr = (t - sum) - y;
        sum = t;
    }

    // Метод Парабол
    double parabola(float a, float b, size_t n)
    {
        if (n % 2 != 0)
        {
            ++n;
        }
        const double h = (b - a) / n;
        double result = m_func(a) + m_func(b);
        double c{};

        #pragma omp parallel for
        for (size_t i = 1; i < n; ++i)
        {
            double coeff = (i % 2 == 0) ? 2.0 : 4.0;
            kahan_sum(coeff * m_func(a + i * h), result, c);
        }
        result *= h / 3.0;
        return result;
    }

public:
    Solver(double (*func)(float))
        : m_func(func) {}

    float integrate_parabola(float a, float b, size_t q)
    {
        return parabola(a, b, q);
    }
};

double func(float x)
{
    return 4. / sqrt(4.f - x * x);
}

const double refer = 2.09439510239319;

int main(int argc, char **argv)
{
    std::cout << "Integral solver\n\n";
    size_t rects = 0;
    if (argc != 2)
    {
        std::cerr << "Usage: integral.exe iterations [default is 1000000000].\n";
        rects = 1000000000;
    }
    else
    {
        rects = std::stoll(argv[1]);
    }

    const float a = 0;
    const float b = 1;

    Solver sol{func};

    auto t_start = std::chrono::high_resolution_clock::now();
    double result = sol.integrate_parabola(a, b, rects);
    auto t_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time = t_end - t_start;

    std::cout << "Elapsed time: " << elapsed_time.count() << std::endl;
    std::cout << "Reference answer: " << refer << std::endl;
    std::cout << "Answer: " << result << std::endl;
    std::cout << "Delta: " << std::abs(refer - result) << std::endl;

    return 0;
}