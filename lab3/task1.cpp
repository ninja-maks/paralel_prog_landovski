#include </usr/include/x86_64-linux-gnu/mpich/mpi.h>
// #include </usr/include/x86_64-linux-gnu/mpich/mpiof.h>
// #include </usr/include/x86_64-linux-gnu/mpich/mpif.h>
// #include </usr/include/x86_64-linux-gnu/mpich/mpio.h>
// #include </usr/include/x86_64-linux-gnu/mpich/mpicxx.h>
#include <iostream>
#include <cmath>


double f(double x) {
    return x * x; 
}


double integrate(double a, double b, int n) {
    double h = (b - a) / n;  
    double sum = 0.0;

    for (int i = 0; i < n; ++i) {
        double x = a + (i + 0.5) * h;  
        sum += f(x);  
    }

    return sum * h;  
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv); 

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
    MPI_Comm_size(MPI_COMM_WORLD, &size); 

    double a = 0.0, b = 10.0;  
    int n = 1000;  
    
    
    int local_n = n / size;
    double local_a, local_b;

    if (rank == 0) {
        
        for (int i = 1; i < size; ++i) {
            local_a = a + i * local_n * ((b - a) / n);
            local_b = a + (i + 1) * local_n * ((b - a) / n);
            MPI_Send(&local_a, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
            MPI_Send(&local_b, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
        }
        
        
        local_a = a;
        local_b = a + local_n * ((b - a) / n);
    } else {
        
        MPI_Recv(&local_a, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&local_b, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    
    double local_result = integrate(local_a, local_b, local_n);

    // Нулевой процесс собирает результаты
    double total_result = 0.0;
    if (rank == 0) {
        total_result = local_result;  
        double recv_result;
        for (int i = 1; i < size; ++i) {
            MPI_Recv(&recv_result, 1, MPI_DOUBLE, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total_result += recv_result;  // Суммируем результаты
        }
    } else {
        // Остальные процессы отправляют результаты
        MPI_Send(&local_result, 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
    }

    if (rank == 0) {
        std::cout << "Total integral result: " << total_result << std::endl;
    }

    MPI_Finalize();  // Завершение MPI
    return 0;
}