CC := gcc
CFLAGS := -Wall -Wextra -O2 -Ilib/ppm_lib -Iimage_processing
LIBCFLAGS := -w

OBJ := main.o ppm.o normalize_image.o
BIN := main

all: $(BIN)

main.o: main.c
	@$(CC) $(CFLAGS) -c $<

ppm.o: lib/ppm_lib/ppm.c
	@$(CC) $(LIBCFLAGS) -Ilib/ppm_lib -c $<

normalize_image.o: image_processing/normalize_image.c
	@$(CC) $(CFLAGS) -c $<

$(BIN): $(OBJ)
	@$(CC) $(OBJ) -o $(BIN)

run: $(BIN)
	@./$(BIN)

clean:
	rm -f @$(BIN) *.o

.INTERMEDIATE: $(OBJ)
