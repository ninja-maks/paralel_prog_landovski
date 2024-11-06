
CC = g++
path_to = 

cpp_var_test:
	$(CC) -g ./cpp_var/new_work.cpp -lpthread
	./a.out


posix_var_test:
	$(CC) -g ./posix_var/work.cpp -lpthread
	./a.out
	rm a.out

mpi_var_test:
	mpicxx ./mpi_var/work.cpp
	mpirun -np 3 --bind-to core ./a.out
	mpirun -np 5 --bind-to core ./a.out
	mpirun -np 7 --bind-to core ./a.out
	mpirun -np 9 --bind-to core ./a.out
	mpirun -np 11 --bind-to core ./a.out
	rm a.out