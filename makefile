OBJ:=$(patsubst %.c, %.o, $(wildcard *.c))
yash.exe: $(OBJ)
	gcc -o $@ $^
clean:
	rm *.o *.exe