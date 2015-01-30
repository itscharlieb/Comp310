bin/shell: shell.c commandlist.c
	gcc bin/shell -o shell.c commandlist.c -I.