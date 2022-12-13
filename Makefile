all: shell hello

shell: shell.c
	gcc -o shell shell.c -I.

hello:hello.c
	gcc -o hello hello.c -I.