#include </usr/include/x86_64-linux-gnu/mpich/mpi.h>
#include <iostream>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 9) {
        if (rank == 0) {
            std::cerr << "This program requires exactly 9 processes.\n";
        }
        MPI_Finalize();
        return -1;
    }

    // Размеры решетки 3x3
    int dims[2] = {3, 3};
    int periods[2] = {0, 1}; // Периодичность по столбцам
    int reorder = 0;

    MPI_Comm newcomm;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, reorder, &newcomm);

    // Получение координат процесса в декартовой топологии
    int coords[2];
    MPI_Cart_coords(newcomm, rank, 2, coords);

    // Создаем коммуникатор для столбцов
    int remain_dims[2] = {1, 0}; // Оставляем только размерность по строкам
    MPI_Comm col_comm;
    MPI_Cart_sub(newcomm, remain_dims, &col_comm);

    // Получаем ранг в коммуникаторе столбца
    int col_rank, col_size;
    MPI_Comm_rank(col_comm, &col_rank);
    MPI_Comm_size(col_comm, &col_size);

    // Определяем соседа сверху и снизу в столбце
    int source = (col_rank - 1 + col_size) % col_size;
    int dest = (col_rank + 1) % col_size;

    // Подготавливаем сообщения
    int send_msg = coords[0]; // Координата строки
    int recv_msg = -1;        // Для полученного сообщения

    // Отправка и прием сообщений
    MPI_Sendrecv(&send_msg, 1, MPI_INT, dest, 0,
                 &recv_msg, 1, MPI_INT, source, 0,
                 col_comm, MPI_STATUS_IGNORE);

    // Вывод результата
    std::cout << "Process: " << rank 
              << " coords: [" << coords[0] << ", " << coords[1] << "]. "
              << "Message = " << recv_msg << std::endl;

    // Завершаем работу
    MPI_Comm_free(&col_comm);
    MPI_Finalize();
    return 0;
}
