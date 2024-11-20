#include <iostream>
#include <cmath>
#include <chrono>
#include <thread>
#include <vector>
#include <omp.h>


double y_eq(double x)
{

    return std::sqrt(std::sqrt(std::pow(std::sqrt(std::pow(std::sqrt(std::pow(std::sqrt(std::pow(2 * x, 2)), 2)), 2)), 2)));
}

double __intergral(double a, double b, int n)
{
     double h = (b - a) / n;
    double temp_res = 0.0;


    #pragma omp parallel for reduction(+:temp_res)
    for (int i = 0; i < n; i++)
    {
        double cur = a + i * h; 
        temp_res += y_eq(cur) * h; 
    }
    return temp_res;
}

double intergral(double a, double b, int n)
{
     double h = (b - a) / n;
    double temp_res = 0.0;


    for (int i = 0; i < n; i++)
    {
        double cur = a + i * h; 
        temp_res += y_eq(cur) * h; 
    }
    return temp_res;
}

void test_thread(double a, double b, double n, int threads_num)
{

    std::cout << "================= " << threads_num << " =================" << std::endl;
    auto start = std::chrono::high_resolution_clock::now();

    double result = 0;
    // ThreadData thread_data = {a, b, b, &result};
    result  = intergral(a, b, (int)n);

    omp_set_num_threads(threads_num); 
    std::cout << "S: " << result << std::endl;
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Time: " << elapsed.count() << " secund" << std::endl;
    result = 0;
    start = std::chrono::high_resolution_clock::now();

    result = __intergral(a, b, (int)n);

    std::cout << "S: " << result << std::endl;
    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;
    std::cout << "Time: " << elapsed.count() << " secund" << std::endl;
}

int main()
{
    setlocale(LC_ALL, "RU");

    double a = 0;
    double b = 10000000;
    int n = 1000000;

    for(int threads: {2,4,8,10}){
        test_thread(a, b, (double)n, threads);
    }


    return 0;
}
