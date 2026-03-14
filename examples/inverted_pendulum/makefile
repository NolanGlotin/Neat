CC = gcc
FLAGS = -Wall -Wextra -Werror -lm
OBJS = graphics.o physics.o

all: main clean

main: $(OBJS)
	$(CC) main.c -g -o main $(FLAGS) $(OBJS) -lraylib -lneat -fsanitize=address

graphics.o: graphics.c
	$(CC) -c graphics.c $(FLAGS)

physics.o: physics.c
	$(CC) -c physics.c $(FLAGS) 

clean:
	rm -f $(OBJS)

run:
	./main