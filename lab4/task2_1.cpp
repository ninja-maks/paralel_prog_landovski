#include <iostream>
#include </usr/include/x86_64-linux-gnu/mpich/mpi.h>

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int dims[2] = {3, 3};    // Размеры сетки 3x3
    const int periods[2] = {0, 1}; // Нециклическая связь по строкам, циклическая по столбцам
    const int reorder = 1;         // Позволяем перестановку рангов для оптимизации
    MPI_Comm newcomm;

    // Создание декартового коммуникатора
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, reorder, &newcomm);

    int coords[2];
    MPI_Cart_coords(newcomm, rank, 2, coords);

    int source, dest;
    int source_tmp;
    // int source_prev, dest_;

    // Сдвиг по оси 0 (строки)

    MPI_Cart_shift(newcomm, 0, 1, &source, &dest);

    if (coords[0] == 0 || coords[0] == 2)
    {
        if (coords[0] == 0)
            source = 6 + coords[1];
        else
            dest = 0 + coords[1];
    }

    // Координаты текущего процесса отправляются соседу по столбцу
    int send_msg = coords[0]; // Отправляем номер строки
    int recv_msg;

    // Отправка и получение сообщения
    MPI_Sendrecv(&send_msg, 1, MPI_INT, dest, 0,
                 &recv_msg, 1, MPI_INT, source, 0,
                 newcomm, MPI_STATUS_IGNORE);

    // Вывод информации

    std::cout << "Process: " << rank
              << " coords: [" << coords[0] << ", " << coords[1] << "]"
      << ". Message = " << recv_msg << "\n";

    MPI_Finalize();
    return 0;
}
