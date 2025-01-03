#include </usr/include/x86_64-linux-gnu/mpich/mpi.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <cmath>
#include <string>

/*

-Максим 19.12: напиши комменты к коду то, что допливал.
        Если думаешь, что по моим не очивидно, то пиши,
        доп комменты напишем.
        Залей в репу до 21.12 плез, чтобы в вс успеть сделать.

Написал комменты, логи надо кажется добавить

Тут база
54:47 : https://www.youtube.com/watch?v=MDG1Fof2LDk

*/

// Функция для инициализации блоков матрицы
void initializeMatrix(std::vector<int> &matrix, int rows, int cols)
{
    for (int i = 0; i < rows * cols; ++i)
    {
        matrix[i] = i + 1;
    }
}

// Функция для печати матрицы в файл
void printMatrixToFile(std::ofstream &file, const std::vector<int> &matrix, int rows, int cols)
{
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            file << std::setw(5) << matrix[i * cols + j];
        }
        file << std::endl;
    }
}

void print_vector(std::vector<int> local_A)
{
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < local_A.size(); ++i)
    {
        oss << local_A[i];
        if (i < local_A.size() - 1)
        {
            oss << ", ";
        }
    }
    oss << "]";
    // std::cout << oss.str() << std::endl;
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // int log_count = 1; // Считает логи

    int N = 3; // Размер матриц (должен делиться на sqrt(size))
    int sqrt_p = static_cast<int>(std::sqrt(size));
    if (sqrt_p * sqrt_p != size || N % sqrt_p != 0)
    {
        if (rank == 0)
        {
            std::cerr << "Количество процессов должно быть квадратом, а размер матриц должен делиться на sqrt(size)." << std::endl;
        }
        MPI_Abort(MPI_COMM_WORLD, -1);
    }

    int block_size = N / sqrt_p;

    // Локальные блоки
    std::vector<int> local_A(block_size * block_size);
    std::vector<int> local_B(block_size * block_size);
    std::vector<int> local_C(block_size * block_size, 0);
    std::fill(local_C.begin(), local_C.end(), 0);

    MPI_Comm grid_comm;
    int dims[2] = {sqrt_p, sqrt_p};
    int periods[2] = {1, 1}; // Замкнутые циклы
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 1, &grid_comm);

    int coords[2];
    MPI_Cart_coords(grid_comm, rank, 2, coords);

    int row_rank, col_rank;
    MPI_Comm row_comm, col_comm;

    MPI_Comm_split(grid_comm, coords[0], coords[1], &row_comm);
    MPI_Comm_split(grid_comm, coords[1], coords[0], &col_comm);

    row_rank = coords[1];
    col_rank = coords[0];

    std::ofstream file("process_" + std::to_string(rank) + ".log");

    if (rank == 0)
    {
        std::vector<int> A(N * N), B(N * N);
        initializeMatrix(A, N, N); // Заполняем матрицу A значениями
        initializeMatrix(B, N, N); // Заполняем матрицу B значениями

        file << "Initial block start:" << std::endl;
        printMatrixToFile(file, A, N, N);
        // Рассылка блоков A и B по всем процессам
        for (int i = 0; i < sqrt_p; ++i)
        {
            for (int j = 0; j < sqrt_p; ++j)
            {
                int dest_rank;
                int tmp[] = {i, j};                        // Массив координат текущего блока (строка, столбец)
                MPI_Cart_rank(grid_comm, tmp, &dest_rank); // Получаем ранг процесса, соответствующего координатам блока

                if (dest_rank != 0)
                {
                    for (int bi = 0; bi < block_size; ++bi)
                    {
                        // Отправляем строку блока A соответствующему процессу
                        MPI_Send(&A[(i * block_size + bi) * N + j * block_size], block_size, MPI_INT, dest_rank, 0, grid_comm);
                        // Отправляем строку блока B соответствующему процессу
                        MPI_Send(&B[(i * block_size + bi) * N + j * block_size], block_size, MPI_INT, dest_rank, 1, grid_comm);
                    }
                }
                else
                {
                    for (int bi = 0; bi < block_size; ++bi)
                    {
                        // Копируем строки блока A и B в локальные массивы
                        std::copy(&A[(i * block_size + bi) * N + j * block_size],
                                  &A[(i * block_size + bi) * N + j * block_size] + block_size,
                                  &local_A[bi * block_size]);

                        std::copy(&B[(i * block_size + bi) * N + j * block_size],
                                  &B[(i * block_size + bi) * N + j * block_size] + block_size,
                                  &local_B[bi * block_size]);
                    }
                }
            }
        }
    }
    else
    {
        for (int bi = 0; bi < block_size; ++bi)
        {
            // Получаем строки блока A от нулевого процесса
            MPI_Recv(&local_A[bi * block_size], block_size, MPI_INT, 0, 0, grid_comm, MPI_STATUS_IGNORE);
            print_vector(local_A);

            // Получаем строки блока B от нулевого процесса
            MPI_Recv(&local_B[bi * block_size], block_size, MPI_INT, 0, 1, grid_comm, MPI_STATUS_IGNORE);
            print_vector(local_B);
        }
    }

    file << "Initial block A:" << std::endl;
    printMatrixToFile(file, local_A, block_size, block_size);

    file << "Initial block B:" << std::endl;
    printMatrixToFile(file, local_B, block_size, block_size);

    // Сдвиги по Кэнону
    // Объявляем переменные для хранения рангов соседей процесса: слева, справа, сверху, снизу.
    int left, right, up, down;
    // Определяем ранги соседей по горизонтали.
    // Второй параметр `1` указывает направление (по оси X).
    // Смещение `-coords[0]` определяет, насколько сдвинуть данные по кольцу.
    // Ранги соседей записываются в `right` (право) и `left` (лево).
    
    left = row_rank == 0 ? rank + 2 : rank - 1;
    right = row_rank == 2 ? rank - 2 : rank + 1;
    up =  col_rank == 0 ? rank + 6 : rank - 3;
    down =  col_rank == 2 ? rank - 6 : rank + 3;

    // Определяем ранги соседей по вертикали.
    // Второй параметр `0` указывает направление (по оси Y).
    // Смещение `-coords[1]` определяет, насколько сдвинуть данные по кольцу.
    // Ранги соседей записываются в `down` (вниз) и `up` (вверх).
    

    file << "l:"<<left << " r:"<< right << " u:"<< up <<" d:" << down << std::endl;  
    file << "col_rank:"<< col_rank << " row_rank:"<< row_rank<< " rank:" << rank <<std::endl;
    // Выполняем обмен блоками матрицы `local_A` между процессами по горизонтали.
    // Данные текущего процесса отправляются соседу слева (`left`) и принимаются от соседа справа (`right`).
    // Функция `MPI_Sendrecv_replace` заменяет содержимое `local_A` на принятые данные.
    MPI_Sendrecv_replace(local_A.data(), block_size * block_size, MPI_INT, left, 0, right, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    // Выполняем обмен блоками матрицы `local_B` между процессами по вертикали.
    // Данные текущего процесса отправляются соседу сверху (`up`) и принимаются от соседа снизу (`down`).
    // Функция `MPI_Sendrecv_replace` заменяет содержимое `local_B` на принятые данные.
    MPI_Sendrecv_replace(local_B.data(), block_size * block_size, MPI_INT, up, 1, down, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    // Итерации алгоритма Кэнона
    // Выполняем `sqrt_p` итераций для полного умножения всех соответствующих блоков матриц A и B.
    
    for (int step = 0; step < sqrt_p; ++step)
    {
        // Умножение локальных блоков
        file << "Calc block C: ";
        for (int i = 0; i < block_size; ++i)
        {
            for (int j = 0; j < block_size; ++j)
            {
                for (int k = 0; k < block_size; ++k)
                {
                    local_C[i * block_size + j] += local_A[i * block_size + k] * local_B[k * block_size + j];
                    file << local_A[i * block_size + k] << " * " << local_B[k * block_size + j] << "+ ";
                    // printMatrixToFile(file, local_C, block_size, block_size);
                }
            }
        }
        file << std::endl;

        // Синхронизация перед сдвигом
        MPI_Barrier(grid_comm);

        // Сдвиг блоков A и B
        MPI_Sendrecv_replace(local_A.data(), block_size * block_size, MPI_INT, left, 0, right, 0, grid_comm, MPI_STATUS_IGNORE);
        MPI_Sendrecv_replace(local_B.data(), block_size * block_size, MPI_INT, up, 1, down, 1, grid_comm, MPI_STATUS_IGNORE);
    }

    file << "Final block C: " << left << " " << right << std::endl;
    printMatrixToFile(file, local_C, block_size, block_size);

    // Итоговая матрица для процесса 0
    std::vector<int> result;
    if (rank == 0)
    {
        result.resize(N * N); // Матрица размером NxN
    }

    // Длины блоков и смещения для каждого процесса
    std::vector<int> sendcounts(size), displs(size);
    if (rank == 0)
    {
        for (int i = 0; i < sqrt_p; ++i)
        {
            for (int j = 0; j < sqrt_p; ++j)
            {
                int proc_rank;
                int coords[] = {i, j};
                MPI_Cart_rank(grid_comm, coords, &proc_rank);

                // Смещение: строка * ширина матрицы * высота блока + столбец * ширину блока
                displs[proc_rank] = i * block_size * N + j * block_size;
                sendcounts[proc_rank] = block_size * block_size;
            }
        }
    }

    // Сборка блоков C в итоговую матрицу
    MPI_Gatherv(
        local_C.data(),          // Отправляемые данные
        block_size * block_size, // Количество элементов, отправляемое каждым процессом
        MPI_INT,                 // Тип данных
        result.data(),           // Итоговый массив на процессе 0
        sendcounts.data(),       // Массив количества элементов от каждого процесса
        displs.data(),           // Смещения каждого блока в итоговой матрице
        MPI_INT,                 // Тип данных
        0,                       // Ранк процесса, который собирает данные
        grid_comm               // Глобальный коммуникатор
    );

    // Печать итоговой матрицы на процессе 0
    if (rank == 0)
    {
        file << "Final matrix:" << std::endl;
        printMatrixToFile(file, result, N, N);
    }

    file.close();

    MPI_Finalize();
    return 0;
}
