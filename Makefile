CC=gcc
OBJS= src/main.o \
	src/inhul_sq_container.o \
	src/inhul_sq_container_group_item.o \
	src/inhul_command.o \
	src/inhul_sq_container_group.o \
	src/inhul_item_data_json.o \
	src/inhul_item_data.o \
	src/inhul_view_model_group.o \
	src/inhul_view_model_item.o \
	src/inhul_view_model_main.o \
	src/gvm_observable_collection.o \
	src/gvm_iterator.o \
	src/gvm_container.o

DATADIR=/usr/share

GTK_CFLAGS=$(shell pkg-config --cflags gtk+-3.0)
GTK_LIBS=$(shell pkg-config --libs gtk+-3.0)

JSON_CFLAGS=$(shell pkg-config --cflags json-glib-1.0)
JSON_LIBS=$(shell pkg-config --libs json-glib-1.0)

LIBS=$(GTK_LIBS) $(JSON_LIBS) -lm
CFLAGS=$(GTK_CFLAGS) $(JSON_CFLAGS) -g -DDATADIR=\"$(DATADIR)\"

all: startMenu

startMenu: $(OBJS)
	$(CC) $(LIBS) $(OBJS) -o startMenu

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o src/$*.o

clean:
	rm src/*.o
