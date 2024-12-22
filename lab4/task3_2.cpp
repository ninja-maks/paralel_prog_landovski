#include </usr/include/x86_64-linux-gnu/mpich/mpi.h>
#include <iostream>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    const int N = 4; // Размер матрицы 4x4
    int matrix[N][N]; // Матрица для хранения данных
    int received_column[N]; // Массив для получения столбца

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

    MPI_Datatype column_type, resized_type;
    MPI_Type_vector(N, 1, N, MPI_INT, &column_type); // Определяем столбец
    MPI_Type_create_resized(column_type, 0, sizeof(int), &resized_type); // Изменяем его тип
    MPI_Type_commit(&resized_type);

    // Нулевой процесс использует MPI_Scatter для отправки данных
    MPI_Scatter(matrix, 1, resized_type, received_column, N, MPI_INT, 0, MPI_COMM_WORLD);

    // Вывод принятого столбца
    std::cout << "Process " << rank << " received column: ";
    for (int i = 0; i < N; i++) {
        std::cout << received_column[i] << " ";
    }
    std::cout << std::endl;

    // Освобождение ресурсов
    MPI_Type_free(&column_type);
    MPI_Type_free(&resized_type);

    MPI_Finalize();

    return 0;
}
