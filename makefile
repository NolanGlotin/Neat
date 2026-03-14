CC = gcc
FLAGS = -Wall -Wextra -Werror -Wno-unused-result -fsanitize=address -O2

OBJS = build/genome.o build/innovation.o build/mutation.o build/neat.o build/network.o build/speciation.o build/utils.o build/config.o
LIB = build/libneat.a
LIBDIR = /usr/local/lib
INCDIR = /usr/local/include/neat

all: build

build/%.o: src/%.c
	mkdir -p build
	$(CC) $(FLAGS) -c $< -o $@

build: $(OBJS)
	ar rcs $(LIB) $(OBJS)

clean:
	rm -rf build

install:
	mkdir -p $(LIBDIR)
	mkdir -p $(INCDIR)
	cp $(LIB) $(LIBDIR)
	cp include/*.h $(INCDIR)

uninstall:
	rm -f $(LIBDIR)/libneat.a
	rm -rf $(INCDIR)