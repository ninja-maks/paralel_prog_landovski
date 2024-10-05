#include <iostream>
#include <cmath>
#include <chrono>
#include <thread>
#include <vector>


double y_eq(double x) {

	return std::sqrt((std::pow(x, 4) - std::tan(std::pow(x, 2))));

}

double intergral(double a, double b, int n) {

	double h = (b - a) / n;
	double cur = a;
	double result = 0;
	while (cur < b) {
		result = y_eq(cur) * h;
		cur += h;
	}
	return result;
}


int main() {
	setlocale(LC_ALL, "RU");
	auto start = std::chrono::high_resolution_clock::now();

	double a =  0;
	double b =  10000;
	int n = 10000000;

	int num_threads = std::thread::hardware_concurrency(); // Получаем количество доступных потоков
    double total_result = 0;
    std::vector<std::thread> threads;

    double part_size = (b - a) / num_threads; // Вычисляем размер части для каждого потока



	std::cout << "S: " << intergral(0, 10000, 10000000)<< std::endl;
	auto end = std::chrono::high_resolution_clock::now();

	std::chrono::duration<double> elapsed = end - start;

	std::cout << "Время выполнения: " << elapsed.count() << " секунд" << std::endl;
	std:: cout << "Кол-во потоков: " << num_threads;
	return 0;

}


