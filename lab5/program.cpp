#include </usr/include/x86_64-linux-gnu/mpich/mpi.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <cmath>
#include <string>
#include <algorithm>
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

// Функция для смещения строк матрицы влево
void shiftRowsLeft(std::vector<int> &matrix, int rows, int cols)
{
    for (int i = 0; i < rows; ++i)
    {
        // Вычисляем начало и конец строки в одномерном векторе
        int start = i * cols;
        int end = start + cols;
        // Выполняем сдвиг влево на i позиций
        std::rotate(matrix.begin() + start, matrix.begin() + start + i, matrix.begin() + end);
    }
}

// Функция для смещения столбцов матрицы вверх
void shiftColumnsUp(std::vector<int> &matrix, int rows, int cols)
{
    for (int j = 0; j < cols; ++j)
    {
        // Извлекаем столбец
        std::vector<int> column(rows);
        for (int i = 0; i < rows; ++i)
        {
            column[i] = matrix[i * cols + j];
        }
        // Сдвигаем столбец вверх на j позиций
        std::rotate(column.begin(), column.begin() + j, column.end());
        // Записываем столбец обратно
        for (int i = 0; i < rows; ++i)
        {
            matrix[i * cols + j] = column[i];
        }
    }
}

// Функция для печати матрицы в файл
void printMatrixToFile(std::ofstream &file, const std::vector<int> &matrix, int rows, int cols)
{
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            file << std::setw(6) << matrix[i * cols + j];
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
    std::cout << oss.str() << std::endl;
}

// Функция для рассылки блоков с использованием пользовательского типа
void distributeBlocks(
    const std::vector<int> &A, const std::vector<int> &B,
    std::vector<int> &local_A, std::vector<int> &local_B,
    int N, int block_size, int sqrt_p, MPI_Comm grid_comm, int rank)
{
    int coords[2];
    MPI_Cart_coords(grid_comm, rank, 2, coords);

    int row = coords[0]; // Координата строки в процессной сетке
    int col = coords[1]; // Координата столбца в процессной сетке

    // Создание пользовательского типа для блока
    MPI_Datatype block_type;
    MPI_Type_vector(block_size, block_size, N, MPI_INT, &block_type);
    MPI_Type_commit(&block_type);

    if (rank == 0)
    {
        for (int i = 0; i < sqrt_p; ++i)
        {
            for (int j = 0; j < sqrt_p; ++j)
            {
                int dest_rank;
                int tmp_coords[] = {i, j};
                MPI_Cart_rank(grid_comm, tmp_coords, &dest_rank);

                // Вычисление начального индекса блока
                int block_start_index = i * block_size * N + j * block_size;

                if (dest_rank != 0)
                {
                    // Отправка блока A
                    MPI_Send(&A[block_start_index], 1, block_type, dest_rank, 0, grid_comm);
                    // Отправка блока B
                    MPI_Send(&B[block_start_index], 1, block_type, dest_rank, 1, grid_comm);
                }
                else
                {
                    // Копирование данных в локальные массивы для rank == 0
                    for (int bi = 0; bi < block_size; ++bi)
                    {
                        std::copy(
                            &A[block_start_index + bi * N],
                            &A[block_start_index + bi * N + block_size],
                            &local_A[bi * block_size]);

                        std::copy(
                            &B[block_start_index + bi * N],
                            &B[block_start_index + bi * N + block_size],
                            &local_B[bi * block_size]);
                    }
                }
            }
        }
    }
    else
    {
        // Получение блоков A и B для других процессов
        MPI_Recv(local_A.data(), block_size * block_size, MPI_INT, 0, 0, grid_comm, MPI_STATUS_IGNORE);
        MPI_Recv(local_B.data(), block_size * block_size, MPI_INT, 0, 1, grid_comm, MPI_STATUS_IGNORE);
    }

    // Освобождение пользовательского типа
    MPI_Type_free(&block_type);
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
    std::vector<int> A(N * N), B(N * N);
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

        initializeMatrix(A, N, N); // Заполняем матрицу A значениями
        initializeMatrix(B, N, N); // Заполняем матрицу B значениями

        file << "Initial block start:" << std::endl;
        printMatrixToFile(file, A, N, N);

        shiftRowsLeft(A, N, N);  // Смещаем A
        shiftColumnsUp(B, N, N); // Смещаем B
        file << "A block start:" << std::endl;
        printMatrixToFile(file, A, N, N);
        file << "B block start:" << std::endl;
        printMatrixToFile(file, B, N, N);
    }

    distributeBlocks(A, B, local_A, local_B, N, block_size, sqrt_p, grid_comm, rank);
    file << "block A:" << std::endl;
    printMatrixToFile(file, local_A, block_size, block_size);

    file << "block B:" << std::endl;
    printMatrixToFile(file, local_B, block_size, block_size);
    // Сдвиги по Кэнону
    // Объявляем переменные для хранения рангов соседей процесса: слева, справа, сверху, снизу.
    int left, right, up, down;

    // Определяем ранги соседей по горизонтали.
    // Второй параметр `1` указывает направление (по оси X).
    // Ранги соседей записываются в `right` (право) и `left` (лево).

    MPI_Cart_shift(grid_comm, 0, 1, &left, &right);

    // Определяем ранги соседей по вертикали.
    // Второй параметр `0` указывает направление (по оси Y).
    // Ранги соседей записываются в `down` (вниз) и `up` (вверх).
    MPI_Cart_shift(grid_comm, 1, 1, &up, &down);

    // file << "l:"<<left << " r:"<< right << " u:"<< up <<" d:" << down << std::endl;
    // file << "col_rank:"<< col_rank << " row_rank:"<< row_rank<< " rank:" << rank <<std::endl;

    // Итерации алгоритма Кэнона
    // Выполняем `sqrt_p` итераций для полного умножения всех соответствующих блоков матриц A и B.

    for (int step = 0; step < sqrt_p; ++step)
    {
        // Compute partial result for the current step
        for (int i = 0; i < block_size; ++i)
        {
            for (int j = 0; j < block_size; ++j)
            {
                
                local_C[i * block_size + j] += local_A[i * block_size + j] * local_B[i * block_size + j];
                
            }
        }
        file << "block C:" << std::endl;
        printMatrixToFile(file, local_C, block_size, block_size);

        // Shift matrix A left within the row communicator
        MPI_Sendrecv_replace(local_A.data(), block_size * block_size, MPI_INT, (coords[1] - 1 + sqrt_p) % sqrt_p, 0,
                             (coords[1] + 1) % sqrt_p, 0, row_comm, MPI_STATUS_IGNORE);

        // Shift matrix B up within the column communicator
        MPI_Sendrecv_replace(local_B.data(), block_size * block_size, MPI_INT, (coords[0] - 1 + sqrt_p) % sqrt_p, 0,
                             (coords[0] + 1) % sqrt_p, 0, col_comm, MPI_STATUS_IGNORE);

        file << "block A:" << std::endl;
        printMatrixToFile(file, local_A, block_size, block_size);

        file << "block B:" << std::endl;
        printMatrixToFile(file, local_B, block_size, block_size);
    }

    file << "Final block C: " << std::endl;
    printMatrixToFile(file, local_C, block_size, block_size);

    // Итоговая матрица для процесса 0
    std::vector<int> result;
    if (rank == 0)
    {
        result.resize(N * N); // Матрица размером NxN
    }

    // Создание пользовательского типа для блока
    MPI_Datatype block_type, resized_block_type;
    MPI_Type_vector(block_size, block_size, N, MPI_INT, &block_type);
    MPI_Type_create_resized(block_type, 0, sizeof(int), &resized_block_type);
    MPI_Type_commit(&resized_block_type);

    // Длины блоков и смещения для каждого процесса
    std::vector<int> sendcounts(size, 1), displs(size);
    if (rank == 0)
    {
        for (int i = 0; i < sqrt_p; ++i)
        {
            for (int j = 0; j < sqrt_p; ++j)
            {
                int proc_rank;
                int coords[] = {i, j};
                MPI_Cart_rank(grid_comm, coords, &proc_rank);

                // Смещение в элементе глобальной матрицы
                displs[proc_rank] = i * block_size * N + j * block_size;
            }
        }
    }

    // Сборка блоков C в итоговую матрицу
    MPI_Gatherv(
        local_C.data(),          // Отправляемые данные
        block_size * block_size, // Количество элементов от каждого процесса
        MPI_INT,                 // Тип данных
        result.data(),           // Итоговый массив на процессе 0
        sendcounts.data(),       // Количество элементов от каждого процесса (в пользовательских типах)
        displs.data(),           // Смещения в итоговом массиве
        resized_block_type,      // Пользовательский тип блока
        0,                       // Ранк процесса, который собирает данные
        grid_comm                // Глобальный коммуникатор
    );
    // Печать итоговой матрицы на процессе 0
    if (rank == 0)
    {
        file << "Final matrix:" << std::endl;
        printMatrixToFile(file, result, N, N);
    }

    file.close();
    MPI_Type_free(&block_type);
    MPI_Type_free(&resized_block_type);
    MPI_Finalize();
    return 0;
}
