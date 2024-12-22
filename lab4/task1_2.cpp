#include </usr/include/x86_64-linux-gnu/mpich/mpi.h>
#include <iostream>
#include <cmath>
#include <vector>
using namespace std;

int main(int argc, char *argv[])
{
    // SetConsoleOutputCP(1251);

    int errCode;

    if ((errCode = MPI_Init(&argc, &argv)) != 0)
    {
        return errCode;
    }

    int world_rank, world_size;
    int *odd, *even;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int oddSize = world_size / 2;
    
    odd = new int[oddSize];
    MPI_Comm odd_comm;
    MPI_Group odd_group, group_world;
    MPI_Comm_group(MPI_COMM_WORLD, &group_world);

      // Определяем цвет для разделения на четные и нечетные группы
    int color = world_rank % 2; // Четные процессы получат цвет 0, нечетные - 1

    // Разделение коммуникатора на две группы: четные и нечетные
    
    MPI_Comm_split(MPI_COMM_WORLD, color, world_rank, &odd_comm);
    
    
    int row_rank, row_size;

    if (color==0)
    {
        MPI_Comm_rank(odd_comm, &row_rank);
        MPI_Comm_size(odd_comm, &row_size);
        char message[30] = "Hello from process ! ";
        message[19] = '0'+ world_rank;
        MPI_Bcast(message, 30, MPI_CHAR, 0, odd_comm);
        
        std::cout << "MPI_COMM_WORLD: " << world_rank
                  << " from " << world_size
                  << ". New comm: " << row_rank << "  from "
                  << row_size 
                  << ". Message = " << message << std::endl;
    }
    else
    {
        row_rank = -1;
        row_size = -1;
    }


    
    if (odd_comm != MPI_COMM_NULL) {
    MPI_Comm_free(&odd_comm);
}

    MPI_Finalize();
    return 0;
}