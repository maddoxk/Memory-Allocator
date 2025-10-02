dumalloc.o: dumalloc.c dumalloc.h
	gcc -c dumalloc.c -o dumalloc.o
	
mallocTest.o: mallocTest.c dumalloc.h
	gcc -c mallocTest.c -o mallocTest.o

out: mallocTest.o dumalloc.o
	gcc mallocTest.o dumalloc.o -o out

clean: rm -f *.o out