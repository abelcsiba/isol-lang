

INCLUDE_DIR=include/

SRC_DIR=src
OBJ_DIR=obj
BIN_DIR=bin

TARGET=$(BIN_DIR)/isolc

SOURCE_FILES=$(wildcard $(SRC_DIR)/*.cc)
OBJECT_FILES=$(SOURCE_FILES:$(SRC_DIR)/%.cc=$(OBJ_DIR)/%.o)

CPPFLAGS=-I$(INCLUDE_DIR) 
CFLAGS=-std=c++23

LDFLAGS=-I$(INCLUDE_DIR)

CC=g++

RM=rm

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECT_FILES) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cc | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	@$(RM) -rv $(BIN_DIR) $(OBJ_DIR)
