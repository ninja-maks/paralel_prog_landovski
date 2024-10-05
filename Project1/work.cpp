#include <iostream>
#include <cmath>
#include <chrono>


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
	std::cout << "S: " << intergral(0, 10000, 100000000)<< "\n";
	auto end = std::chrono::high_resolution_clock::now();

	std::chrono::duration<double> elapsed = end - start;

	std::cout << "Время выполнения: " << elapsed.count() << " секунд" << std::endl;
	return 0;

}


