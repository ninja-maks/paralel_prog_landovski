#include </usr/include/x86_64-linux-gnu/mpich/mpi.h>
#include <iostream>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    const int N = 4; // Размер матрицы 4x4
    int matrix[N][N]; // Матрица для хранения данных

    if (rank == 0) {
        // Инициализация матрицы в нулевом процессе и вывод для нагладности
        int value = 1;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                matrix[i][j] = value++;
                std::cout << matrix[i][j] << "\t";
            }
            std::cout << std::endl;
        }

    }

    // Создание производного типа данных для элементов главной диагонали и ниже
    MPI_Datatype lower_triangle_type;
    int block_lengths[N];   // Длины блоков
    int displacements[N];   // Смещения элементов

    for (int i = 0; i < N; i++) {
        block_lengths[i] = i + 1; // Количество элементов в каждом ряду (до главной диагонали включительно)
        displacements[i] = i * N; // Смещение начала ряда в матрице
    }

    // Создаем производный тип
    MPI_Type_indexed(N, block_lengths, displacements, MPI_INT, &lower_triangle_type);
    MPI_Type_commit(&lower_triangle_type);

    if (rank == 0) {
        // Нулевой процесс отправляет данные первому
        MPI_Send(matrix, 1, lower_triangle_type, 1, 0, MPI_COMM_WORLD);
    } else if (rank == 1) {
        // Первый процесс принимает данные
        int received_elements[N]; // Массив для хранения элементов
        MPI_Recv(received_elements, 1, lower_triangle_type, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Вывод принятых элементов
        std::cout << "Process "<<rank<<" received elements: ";
        for (int i = 0; i <N; i++) {
            std::cout << received_elements[ i*(N+1) ] << " ";
        }
        std::cout << std::endl;
    }

    // Освобождение ресурсов
    MPI_Type_free(&lower_triangle_type);
    MPI_Finalize();

    return 0;
}
