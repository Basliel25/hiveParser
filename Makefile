CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -pthread -std=gnu11 -g

SRC = $(wildcard src/*.c src/log_parser/*.c src/report_parse/*.c)
OBJ = $(SRC:.c=.o)
TARGET = hiveParse

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean: $(TARGET)
	rm -f $(TARGET) $(OBJ)

docs:
	doxygen docs/Doxyfile
	@echo "Docs generated at docs/html/index.html"

tidy:
	clang-tidy $(SRC) 
