#include <iostream>
#include <fstream>
#include <cstdlib> // для rand()
#include <ctime>   // для time()

int main() {
    const int numberOfTasks = 1000; // Количество записей
    std::ofstream outputFile("tasks.txt"); // Открываем файл для записи

    // Инициализация генератора случайных чисел
    std::srand(static_cast<unsigned int>(std::time(0)));

    if (!outputFile.is_open()) {
        std::cerr << "Не удалось открыть файл для записи." << std::endl;
        return 1;
    }

    for (int i = 1; i <= numberOfTasks; ++i) {
        int x = std::rand() % 10000; // Генерация случайного числа от 0 до 9999
        int y = std::rand() % 10000; // Генерация случайного числа от 0 до 9999
        outputFile << i << ". " << x << " " << y << std::endl; // Запись в файл
    }

    outputFile.close(); // Закрываем файл
    std::cout << "Записи успешно сгенерированы в tasks.txt!" << std::endl;

    return 0;
}