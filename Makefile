CC=gcc
OBJS=src/main.o src/container.o src/data_item.o src/item.o src/command.o

GTK_CFLAGS=$(shell pkg-config --cflags gtk+-3.0)
GTK_LIBS=$(shell pkg-config --libs gtk+-3.0)

LIBS=$(GTK_LIBS)
CFLAGS=$(GTK_CFLAGS) -g

all: startMenu

startMenu: $(OBJS)
	$(CC) $(LIBS) $(OBJS) -o startMenu

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o src/$*.o

clean:
	rm src/*.o
