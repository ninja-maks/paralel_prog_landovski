

test_task1_1:
	mpicxx ./task1_1.cpp -pthread -fexceptions
	mpirun -np 4 --bind-to core ./a.out 
	rm ./a.out


test_task1_2:
	mpicxx ./task1_2.cpp -pthread -fexceptions
	mpirun -np 4 --bind-to core ./a.out 
	rm ./a.out

test_task2_1:
	mpicxx ./task2_1.cpp -pthread -fexceptions
	mpirun -np 9 --bind-to core ./a.out
	rm ./a.out

test_task2_2:
	mpicxx ./task2_2.cpp -pthread -fexceptions
	mpirun -np 9 --bind-to core ./a.out
	rm ./a.out

test_task3_1:
	mpicxx ./task3_1.cpp -pthread -fexceptions
	mpirun -np 9 --bind-to core ./a.out
	rm ./a.out

test_task3_2:
	mpicxx ./task3_2.cpp -pthread -fexceptions
	mpirun -np 3 --bind-to core ./a.out
	rm ./a.out

test_task3_3:
	mpicxx ./task3_3.cpp -pthread -fexceptions
	mpirun -np 4 --bind-to core ./a.out
	rm ./a.out