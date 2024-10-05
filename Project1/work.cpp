#include <iostream>
#include <cmath>
#include <chrono>
#include <thread>
#include <vector>


double y_eq(double x) {

	return std::sqrt(std::powf((double)x, 4));

}

void intergral(double a, double b, double n, double* result) {
	*result = 0;
	double h = (b - a) / n;
	double cur = a;
	while (cur < b) {
		*result += y_eq(cur) * h;
		cur += h;
	}
}

void integral_thread(double a, double b, double n, double*total_result) {
	int num_threads = 0;
	std::vector<std::thread> threads;
	double h = ((b - a) / n);
	// Размер части для каждого потока
	double part_size = (b - a) / num_threads  - fmod((b - a) / num_threads,h) ;



	for (int i = 0; i < num_threads; ++i) {
		double thread_a = a + i * part_size;
		double thread_b = (i == (num_threads - 1)) ? b : (thread_a + part_size); // Чтобы последний поток достигал b
		double n_this = n / num_threads;
		threads.emplace_back(intergral, thread_a, thread_b, n_this, total_result);
	}

	// Ожидание окончания потоков
	for (std::thread& t : threads) {
		t.join();
	}


}


int main() {
	setlocale(LC_ALL, "RU");
	

	double a =  0;
	double b =  10;
	int n = 10000;

	auto start = std::chrono::high_resolution_clock::now();
	double result;
	intergral(a, b, n, &result);
	std::cout << "S: " << result<< std::endl;
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = end - start;
	std::cout << "Time: " << elapsed.count() << " secund" << std::endl;

	start = std::chrono::high_resolution_clock::now();
	integral_thread(a, b, n, &result);
	std::cout << "S: " << result << std::endl;
	end = std::chrono::high_resolution_clock::now();
	elapsed = end - start;
	std::cout << "Time: " << elapsed.count() << " secund" << std::endl;
	
	
	return 0;

}


