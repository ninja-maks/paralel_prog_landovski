#include <iostream>
#include <cmath>
#include <chrono>
#include <thread>
#include <vector>
#include <mutex>

std::mutex mtx;

double y_eq(double x) {

	return std::sqrt(std::sqrt(std::pow(std::sqrt(std::pow(std::sqrt(std::pow(std::sqrt( std::pow(2 * x, 2)), 2)), 2)), 2)));

}

void intergral(double a, double b, double n, double* result) {
	double h = (b - a) / n;
	double cur = a;
	double temp_res = 0;
	while (cur < b) {
		temp_res += y_eq(cur) * h;
		cur += h;
	}
	std::lock_guard<std::mutex> guard(mtx);
	*result += temp_res;
	/*std::cout << "thread_a is " << a << std::endl;
	std::cout << "thread_b is " << b << std::endl;
	std::cout << "n_this is " << n << std::endl;
	std::cout << "res_tmp is " << temp_res  << " res_cur is " << *result << std::endl;*/
	//std::cout << "res is " << *result << std::endl;
}

void integral_thread(double a, double b, double n, double* total_result) {
	int num_threads = 6;
	std::vector<std::thread> threads;
	double h = ((b - a) / n);
	// Размер части для каждого потока
	
	int steps_count = n / num_threads;


	for (int i = 0; i < num_threads; i++) {
		double n_this = (i == (num_threads - 1)) ? n - (steps_count *num_threads) + steps_count : steps_count;
		double thread_a = a + i * h* steps_count;
		double thread_b = (i == (num_threads - 1)) ? b : (thread_a + (h* n_this)); // Чтобы последний поток достигал b
		
		/*std::cout << "thread_a is " << thread_a << std::endl;
		std::cout << "thread_b is " << thread_b << std::endl;
		std::cout << "n_this is " << n_this << std::endl;*/

		threads.emplace_back(intergral, thread_a, thread_b, n_this, total_result);
	}

	// Ожидание окончания потоков
	for (std::thread& t : threads) {
		t.join();
	}


}


int main() {
	setlocale(LC_ALL, "RU");


	double a = 0;
	double b = 1000000;
	int n = 10000000;

	auto start = std::chrono::high_resolution_clock::now();
	double result = 0;
	intergral(a, b, n, &result);
	std::cout << "S: " << result << std::endl;
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = end - start;
	std::cout << "Time: " << elapsed.count() << " secund" << std::endl;
	result = 0;
	start = std::chrono::high_resolution_clock::now();
	integral_thread(a, b, n, &result);
	std::cout << "S: " << result << std::endl;
	end = std::chrono::high_resolution_clock::now();
	elapsed = end - start;
	std::cout << "Time: " << elapsed.count() << " secund" << std::endl;


	return 0;

}


