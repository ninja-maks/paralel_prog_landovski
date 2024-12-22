#include </usr/include/x86_64-linux-gnu/mpich/mpi.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <chrono>

double f(double x)
{
    return x * x;
}

double integrate(double a, double b, int n)
{
    double h = (b - a) / n;
    double sum = 0.0;

    for (int i = 0; i < n; ++i)
    {
        double x = a + (i + 0.5) * h;
        sum += f(x);
    }

    return sum * h;
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double a = 0.0, b = 10.0;
    int n = 1000;

    int local_n = n / size; // Локальное количество интервалов для каждого процесса

    double local_a[size];
    double local_b[size];

    double local_result = 0.0;

    auto start = std::chrono::high_resolution_clock::now();
    // Нулевой процесс вычисляет границы интервалов
    if (rank == 0)
    {
        std::cout << "================= " << size << " =================" << std::endl;
    }

    for (int i = 0; i < size; ++i)
    {
        local_a[i] = a + i * (b - a) / size;       // Начальная граница
        local_b[i] = a + (i + 1) * (b - a) / size; // Конечная граница
    }

    // Каждый процесс вычисляет свой локальный результат интегрирования
    local_result = integrate(local_a[rank], local_b[rank], local_n);

    // Нулевой процесс собирает результаты
    double total_result;
    MPI_Reduce(&local_result, &total_result, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    // Нулевой процесс суммирует результаты
    if (rank == 0)
    {
        std::cout << "Total integral result: " << total_result << std::endl;

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "Time: " << elapsed.count() << " secund" << std::endl;
        std::cout << "================= " << "END" << " =================" << std::endl;
    }

    MPI_Finalize(); // Завершение MPI
    return 0;
}
