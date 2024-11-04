
CC = g++
path_to = 

cpp_var_test:
	$(CC) -g ./cpp_var/work.cpp -lpthread
	./a.out
	rm a.out

posix_var_test:
	$(CC) -g ./posix_var/work.cpp -lpthread
	./a.out
	rm a.out

mpi_var_test:
	mpicxx -g ./mpi_var/work.cpp
	./a.out
	rm a.out