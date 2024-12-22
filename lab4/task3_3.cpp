#include </usr/include/x86_64-linux-gnu/mpich/mpi.h>
#include <iostream>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int N = 4;  // Размер матрицы 4x4
    int matrix[N][N]; // Матрица в процессе 0
    int received_block[4]; // Блок 2x2 для получения

    if (rank == 0) {
        // Инициализация матрицы в нулевом процессе
        std::cout << "Process: " << rank << std::endl;
        int value = 1;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                matrix[i][j] = value++;
                std::cout << matrix[i][j]<<"\t";
            }
            std::cout << std::endl;
        }

    }

    // Создание производного типа данных для блока 2x2
    MPI_Datatype block_type, resized_type;
    int block_lengths[2] = {2, 2};
    int block_displacements[2] = {0, N}; // Смещения для блоков по строкам
    MPI_Type_indexed(2, block_lengths, block_displacements, MPI_INT, &block_type);
    MPI_Type_create_resized(block_type, 0, sizeof(int), &resized_type); // Учитываем размер элемента
    MPI_Type_commit(&resized_type);

    // Настройка параметров для MPI_Scatterv
    int send_counts[4] = {0, 1, 1, 1}; // Нулевой процесс отправляет только первым трем
    int displacements[4] = {0, 0, N-2, N*(N-1)-2}; // Смещения для блоков

    MPI_Scatterv(rank == 0 ? &matrix[0][0] : nullptr, 
                 send_counts, 
                 displacements, 
                 resized_type, 
                 received_block, 
                 4, 
                 MPI_INT, 
                 0, 
                 MPI_COMM_WORLD);

    // Вывод результата
    if (rank != 0) {
        std::cout << "Process: " << rank << ". Message = {";
        for (int i = 0; i < 4; i++) {
            if(i==0)std::cout << "{";
            if(i==2)std::cout << "}, {";
            std::cout << received_block[i] << (i==2 || i == 0? ", " : "");
            if(i==3)std::cout << "}";
        }
        std::cout << "}"<< std::endl;
    }

    // Освобождение ресурсов
    MPI_Type_free(&block_type);
    MPI_Type_free(&resized_type);

    MPI_Finalize();
    return 0;
}
