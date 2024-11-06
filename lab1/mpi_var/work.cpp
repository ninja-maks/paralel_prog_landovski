#include <iostream>
#include <cmath>
#include <chrono>
#include <pthread.h>
#include <vector>
#include <mutex>

#include </usr/include/x86_64-linux-gnu/mpich/mpi.h>

std::mutex mtx;

struct ThreadData
{
    double a;
    double b;
    double n;
    double *total_result;
};

struct tmp
{
    int argc;
    char **argv;
    int rank, size;
};

static tmp stat_mem;

void *integral(void *arg);

double y_eq(double x)
{

    return std::sqrt(std::sqrt(std::pow(std::sqrt(std::pow(std::sqrt(std::pow(std::sqrt(std::pow(2 * x, 2)), 2)), 2)), 2)));
}

void *integral(void *arg)
{
    ThreadData *data = static_cast<ThreadData *>(arg);
    // {thread_a, thread_b, n_this, total_result}
    double n = data->n;
    double a = data->a;
    double b = data->b;
    double *result = data->total_result;
    double h = (b - a) / n;
    double cur = a;
    double temp_res = 0;
    while (cur < b)
    {
        temp_res += y_eq(cur) * h;
        cur += h;
    }
    std::lock_guard<std::mutex> guard(mtx);
    *result += temp_res;
    return nullptr;
}

void integral_thread(double a, double b, double n, double *total_result, int threads_num)
{
    *total_result = 0;
    int num_threads = threads_num;
    std::vector<ThreadData> thread_data(num_threads);

    double steps_count = n / num_threads;
    double n_this;
    double thread_a;
    double thread_b;
    if (stat_mem.rank == 0)
    {
        for (int i = 0; i < num_threads; i++)
        {
            n_this = (i == (num_threads - 1)) ? n - (steps_count * (num_threads - 1)) : steps_count;
            thread_a = a + i * (b - a) / n * steps_count;
            thread_b = (i == (num_threads - 1)) ? b : (thread_a + (b - a) / n * n_this);

            // Заполнение структуры данными для потока
            MPI_Send(&thread_a, 1, MPI_DOUBLE, i+1, 0, MPI_COMM_WORLD);
            MPI_Send(&thread_b, 1, MPI_DOUBLE, i+1, 0, MPI_COMM_WORLD);
            MPI_Send(&n_this, 1, MPI_DOUBLE, i+1, 0, MPI_COMM_WORLD);
        }
    }else{
    MPI_Recv(&thread_a, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&thread_b, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&n_this, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    }

    
    double result = 0;
    ThreadData it = {thread_a, thread_b, n_this, &result};
    integral(&it);

    

    *total_result = 0;
    if (stat_mem.rank == 0)
    {
        for (int i = 0; i < num_threads; i++)
        {
            MPI_Recv(&result, 1, MPI_DOUBLE, i+1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            *total_result += result;
        }
    }else{
        MPI_Send(&result, 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
    }
}

void test_thread(double a, double b, double n, int threads_num)
{
    

    std::chrono::_V2::system_clock::time_point start;
    std::chrono::_V2::system_clock::time_point end;
    std::chrono::duration<double> elapsed;
    double result;
    if (stat_mem.rank == 0)
    {
        std::cout << "================= " << threads_num << " =================" << std::endl;

        start = std::chrono::high_resolution_clock::now();

        result = 0;
        ThreadData thread_data = {a, b, b, &result};

        integral(&thread_data);

        std::cout << "S: " << *thread_data.total_result << std::endl;
        end = std::chrono::high_resolution_clock::now();
        elapsed = end - start;
        std::cout << "Time: " << elapsed.count() << " secund" << std::endl;

        result = 0;
        start = std::chrono::high_resolution_clock::now();
    }
    
    result=0;
    MPI_Barrier(MPI_COMM_WORLD); 
    integral_thread(a, b, n, &result, threads_num);

    if (stat_mem.rank == 0)
    {
        std::cout << "S: " << result << std::endl;
        end = std::chrono::high_resolution_clock::now();
        elapsed = end - start;
        std::cout << "Time: " << elapsed.count() << " secund" << std::endl;
    }
//      std::cout << "All processes are synchronized."<< " rank:" << stat_mem.rank <<"\n";
//    MPI_Barrier(MPI_COMM_WORLD); 
    
}

int main(int argc, char **argv)
{
    stat_mem.argc = argc;
    stat_mem.argv = argv;



    MPI_Init(&stat_mem.argc, &stat_mem.argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &stat_mem.rank);
    MPI_Comm_size(MPI_COMM_WORLD, &stat_mem.size);

    setlocale(LC_ALL, "RU");
    // std::cout << "size:" << stat_mem.size << std::endl;
    double a = 0;
    double b = 10000000;
    int n = 1000000;

    test_thread(a, b, (double)n, stat_mem.size-1);

    MPI_Finalize();
    return 0;
}
