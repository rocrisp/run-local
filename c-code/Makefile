CC = gcc
CFLAGS = -Wall -Wextra -pthread -I./include -std=c11 -O2
LDFLAGS = -pthread -lm

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
INC_DIR = include

SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
TARGET = $(BIN_DIR)/scheduler

.PHONY: all clean run debug directories

all: directories $(TARGET)

directories:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR) logs config

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Build complete: $(TARGET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

debug: CFLAGS += -g -DDEBUG
debug: clean all

clean:
	rm -rf $(OBJ_DIR)/*.o $(TARGET)
	@echo "Clean complete"

run: all
	@./$(TARGET)

help:
	@echo "Available targets:"
	@echo "  all     - Build the project (default)"
	@echo "  clean   - Remove build artifacts"
	@echo "  run     - Build and run the scheduler"
	@echo "  debug   - Build with debug symbols"
	@echo "  help    - Show this help message"
