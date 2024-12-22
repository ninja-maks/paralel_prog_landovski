


CFLAGS += $(shell mpicc -showme:compile)
LDFLAGS += $(shell mpicc -showme:link)

test_task1:
	mpicxx ./task1.cpp -pthread -fexceptions
	mpirun -np 1 --bind-to core ./a.out
	mpirun -np 2 --bind-to core ./a.out
	mpirun -np 3 --bind-to core ./a.out
	mpirun -np 4 --bind-to core ./a.out
	mpirun -np 5 --bind-to core ./a.out
	mpirun -np 6 --bind-to core ./a.out
	mpirun -np 7 --bind-to core ./a.out
	mpirun -np 8 --bind-to core ./a.out
	mpirun -np 9 --bind-to core ./a.out
	mpirun -np 10 --bind-to core ./a.out
	rm ./a.out

test_task2:
	mpicxx ./task2.cpp -pthread -fexceptions
	mpirun -np 1 --bind-to core ./a.out
	mpirun -np 2 --bind-to core ./a.out
	mpirun -np 3 --bind-to core ./a.out
	mpirun -np 4 --bind-to core ./a.out
	mpirun -np 5 --bind-to core ./a.out
	mpirun -np 6 --bind-to core ./a.out
	mpirun -np 7 --bind-to core ./a.out
	mpirun -np 8 --bind-to core ./a.out
	mpirun -np 9 --bind-to core ./a.out
	mpirun -np 10 --bind-to core ./a.out
	rm ./a.out

test_task3:
	mpicxx ./task3.cpp -pthread -fexceptions
	mpirun -np 1 --bind-to core ./a.out
	mpirun -np 2 --bind-to core ./a.out
	mpirun -np 3 --bind-to core ./a.out
	mpirun -np 4 --bind-to core ./a.out
	mpirun -np 5 --bind-to core ./a.out
	mpirun -np 6 --bind-to core ./a.out
	mpirun -np 7 --bind-to core ./a.out
	mpirun -np 8 --bind-to core ./a.out
	mpirun -np 9 --bind-to core ./a.out
	mpirun -np 10 --bind-to core ./a.out
	rm ./a.out
