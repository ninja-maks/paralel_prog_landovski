#include <iostream>
#include <cmath>
#include <chrono>
#include <pthread.h>
#include <vector>
#include <mutex>

std::mutex mtx;

struct ThreadData {
    double a;
    double b;
    double n;
    double* total_result;
};

void* integral(void* arg);

double y_eq(double x) {

	return std::sqrt(std::sqrt(std::pow(std::sqrt(std::pow(std::sqrt(std::pow(std::sqrt( std::pow(2 * x, 2)), 2)), 2)), 2)));

}

void* integral(void* arg) {
	ThreadData* data = static_cast<ThreadData*>(arg);
	// {thread_a, thread_b, n_this, total_result}
	double n = data->n;
	double a = data->a;
	double b = data->b;
	double *result = data->total_result; 
	double h = (b - a) / n;
	double cur = a;
	double temp_res = 0;
	while (cur < b) {
		temp_res += y_eq(cur) * h;
		cur += h;
	}
	std::lock_guard<std::mutex> guard(mtx);
	*result += temp_res;
	return nullptr;
	/*std::cout << "thread_a is " << a << std::endl;
	std::cout << "thread_b is " << b << std::endl;
	std::cout << "n_this is " << n << std::endl;
	std::cout << "res_tmp is " << temp_res  << " res_cur is " << *result << std::endl;*/
	//std::cout << "res is " << *result << std::endl;
}

void integral_thread(double a, double b, double n, double* total_result) {
    int num_threads = 6;
    std::vector<pthread_t> threads;
    std::vector<ThreadData> thread_data(num_threads);
    
    double steps_count = n / num_threads;
  
    for (int i = 0; i < num_threads; i++) {
        double n_this = (i == (num_threads - 1)) ? n - (steps_count * (num_threads - 1)) : steps_count;
        double thread_a = a + i * (b - a) / n * steps_count;
        double thread_b = (i == (num_threads - 1)) ? b : (thread_a + (b - a) / n * n_this);
        
        // Заполнение структуры данными для потока
        thread_data[i] = {thread_a, thread_b, n_this, total_result};
        
        // Создаем поток
        pthread_create(&threads[i], nullptr, integral, (void*)&thread_data[i]);
    }

    // Ожидаем окончания потоков
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], nullptr);
    }
}


int main() {
	setlocale(LC_ALL, "RU");


	double a = 0;
	double b = 1000000;
	int n = 10000000;
	
	auto start = std::chrono::high_resolution_clock::now();
	double result = 0;
	ThreadData thread_data = {a, b, b, &result};
	integral(&thread_data);
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


