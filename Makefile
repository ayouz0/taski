# Makefile for Task Manager with Tree Support

CC = gcc
CFLAGS = -Wall -Wextra
LIBS = -lncurses

# Project structure
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
BIN_DIR = bin

# Source files
SRCS = $(SRC_DIR)/main.c \
       $(SRC_DIR)/ui_core.c \
       $(SRC_DIR)/ui_menu.c \
       $(SRC_DIR)/ui_message.c \
       $(SRC_DIR)/task_core.c \
       $(SRC_DIR)/task_add.c \
       $(SRC_DIR)/task_view.c \
       $(SRC_DIR)/task_edit.c \
       $(SRC_DIR)/task_delete.c \
       $(SRC_DIR)/component.c \
       $(SRC_DIR)/fileio_task.c \
       $(SRC_DIR)/fileio_component.c \
       $(SRC_DIR)/tree_parser.c \
       $(SRC_DIR)/tree_extract.c

# Object files
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

# Target executable
TARGET = $(BIN_DIR)/task_manager

# Create directories if they don't exist
DIRS = $(OBJ_DIR) $(BIN_DIR)
$(shell mkdir -p $(DIRS))

# Default target
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I $(INC_DIR) -c $< -o $@

# Clean up build files
clean:
	rm -rf $(OBJ_DIR)/*.o $(TARGET)

# Run the program
run: $(TARGET)
	$(TARGET)
re : clean run

# Create example tree file directory
setup:
	mkdir -p example_trees
	@echo "[PARSER]\nTokenizer implementation #high @student1\nAST generation #medium\nSyntax validation\nHandle quotation marks @student2\n\n[EXECUTION]\nProcess creation #high\nPipe handling @student1\nRedirections #medium @student2\n\n[BUILTINS]\ncd implementation #low\necho implementation\nexport implementation @student1" > example_trees/minishell.tree
	@echo "Setup complete. Example tree file created at example_trees/minishell.tree"

.PHONY: all clean run setup