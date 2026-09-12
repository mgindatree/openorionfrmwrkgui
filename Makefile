CC = gcc

# GTK4 fix
PKG_CONFIG_PATH := /usr/lib/x86_64-linux-gnu/pkgconfig
export PKG_CONFIG_PATH

CFLAGS := -Wall -Wextra -g \
          $(shell pkg-config --cflags gtk4) 

LIBS := $(shell pkg-config --libs gtk4)

SRC_DIR := src
OBJ_DIR := obj
OUT_DIR := output
TARGET := $(OUT_DIR)/main

SOURCES := $(wildcard $(SRC_DIR)/*.c)
OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))

all: $(TARGET)

$(TARGET): $(OBJECTS) | $(OUT_DIR)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET) $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OUT_DIR):
	mkdir -p $(OUT_DIR)

clean:
	rm -rf $(OBJ_DIR) $(OUT_DIR)

run: all
	./$(TARGET)