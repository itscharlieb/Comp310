binpath = bin
headerpath = include
srcpath = src
testpath = test

all: $(binpath)/memory_manager.o test

test: $(binpath)/memory_manager.o $(testpath)/test.c
	gcc -I$(headerpath) $(binpath)/memory_manager.o $(testpath)/test.c -o $(binpath)/test

$(binpath)/memory_manager.o: $(srcpath)/memory_manager.c $(headerpath)/memory_manager.h
	gcc -I$(headerpath) -c $(srcpath)/memory_manager.c -o $(binpath)/memory_manager.o

clean:
	rm $(binpath)/*
