all: writer reader
writer:
	gcc -g -Wall -o writer writer.c -pthread -lrt -c
reader:
	gcc -g -Wall -o reader reader.c -lrt -pthread -c
clean:
	rm -rf writer 
	rm -rf reader

