shell: shell.o commandlist.o
	gcc -o bin/shell shell.o commandlist.o

shell.o: shell.c commandlist.h
	gcc -c shell.c -I.

commandlist.o: commandlist.c commandlist.h
	gcc -c commandlist.c -I.
