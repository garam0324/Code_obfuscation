CC      := gcc
CFLAGS  := -Wall -Wextra -O2 -std=c11
LDFLAGS :=

TARGET  := license_obf
SRC     := license_obf.c
OBJ     := $(SRC:.c=.o)

.PHONY: all clean run debug

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

run: $(TARGET)
	./$(TARGET)

debug: CFLAGS := -Wall -Wextra -O0 -g -std=c11
debug: clean $(TARGET)

clean:
	rm -f $(TARGET) $(OBJ)
