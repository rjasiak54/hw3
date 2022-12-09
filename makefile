

test: test.c
	gcc -std=c11 -g test.c -o test


main:
	mpicc main.c -o main

run:
	mpirun --np $(NP) main

clean:
	rm -f test main