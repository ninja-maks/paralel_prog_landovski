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
        // std::cout << "one thread: " << integrate(a,b,n) << std::endl;
        for (int i = 0; i < size; ++i)
        {
            local_a[i] = a + i * (b - a) / size;       // Начальная граница
            local_b[i] = a + (i + 1) * (b - a) / size; // Конечная граница
        }
    }
    double l_a; // Переменная для получения значения от начала промежутка
    double l_b; // Переменная для получения значения от конца промежутка
    // Рассылаем границы интервалов всем процессам
    MPI_Scatter(local_a, 1, MPI_DOUBLE, &l_a, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(local_b, 1, MPI_DOUBLE, &l_b, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Каждый процесс вычисляет свой локальный результат интегрирования
    local_result = integrate(l_a, l_b, local_n);

    // Нулевой процесс собирает результаты
    double total_result[size];
    MPI_Gather(&local_result, 1, MPI_DOUBLE, &total_result, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Нулевой процесс суммирует результаты
    if (rank == 0)
    {
        double tmp = 0;
        for (int i = 0; i < size; ++i)
        {
            tmp += total_result[i]; // Считаем общую сумму всех результатов
        }
        std::cout << "Total integral result: " << tmp << std::endl;

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "Time: " << elapsed.count() << " secund" << std::endl;
        std::cout << "================= " << "END" << " =================" << std::endl;
    }

    MPI_Finalize(); // Завершение MPI
    return 0;
}
