

test:
	mpicxx ./program.cpp -pthread -fexceptions
	mpirun -np 9 --bind-to core ./a.out
	rm a.out


clean:
	rm process*