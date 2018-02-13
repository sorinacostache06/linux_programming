all: binary_sem.o writer.o reader.o writer reader 
binary_sem.o:
	gcc -c binary_sem.c -o binary_sem.o	
writer.o:
	gcc -c writer.c -o writer.o
reader.o:
	gcc -c reader.c -o reader.o
writer:
	gcc writer.o binary_sem.o -o writer -lrt
reader:
	gcc reader.o binary_sem.o -o reader -lrt 
clean:
	rm -rf writer 
	rm -rf reader
	rm -rf writer.o
	rm -rf reader.o
	rm -rf binary_sem.o
