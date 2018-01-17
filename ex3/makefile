all: writer reader
writer:
	gcc -g -Wall -o writer writer.c -pthread -lrt 
reader:
	gcc -g -Wall -o reader reader.c -lrt -pthread
clean:
	rm -rf writer 
	rm -rf reader

