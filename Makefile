# Operating system
ifeq ($(OS), Windows_NT)
    PLATFORM_OS =Windows
else
    PLATFORM_OS =Linux
endif

# Directories
INC_DIR =include
SRC_DIR =src
OBJ_DIR =obj

# Files
EXEC     =game
SRCS     =$(wildcard $(SRC_DIR)/*.c)
OBJS     =$(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
MAIN_SRC =main.c
MAIN_OBJ =$(OBJ_DIR)/main.o

## Append .exe extension for Windows
ifeq ($(PLATFORM_OS), Windows)
	EXEC :=$(EXEC).exe
endif

# GNU Make variables
CPPFLAGS =$(addprefix -I,$(INC_DIR))
CFLAGS   =-Wall -Wextra -Werror
LDLIBS   =-lraylib

ifeq ($(PLATFORM_OS), Linux)
	LDLIBS +=-lm
endif
ifeq ($(PLATFORM_OS), Windows)
	LDLIBS +=-lgdi32 -lwinmm
endif

# Rules

## Default target (build)
all: $(OBJ_DIR) $(EXEC)

## Create objects folder
$(OBJ_DIR):
ifeq ($(PLATFORM_OS), Linux)
	@mkdir -p $(OBJ_DIR)
endif
ifeq ($(PLATFORM_OS), Windows)
	@mkdir $(OBJ_DIR)
endif

## Linking
$(EXEC): $(OBJS) $(MAIN_OBJ)
	$(CC) $^ $(LDLIBS) -o $(EXEC)
	@echo $(EXEC) was created successfully!

## Compiling
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $< $(LDLIBS) -o $@

$(MAIN_OBJ): $(MAIN_SRC)
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $< $(LDLIBS) -o $@

## Clean up
clean:
ifeq ($(PLATFORM_OS), Linux)
	@rm -rf $(OBJ_DIR)
endif
ifeq ($(PLATFORM_OS), Windows)
	@if exist $(OBJ_DIR) rd /s /q $(OBJ_DIR)
endif

fclean:
ifeq ($(PLATFORM_OS), Linux)
	@rm -rf $(OBJ_DIR) $(EXEC)
endif
ifeq ($(PLATFORM_OS), Windows)
	@if exist $(OBJ_DIR) rd /s /q $(OBJ_DIR)
	@if exist $(EXEC) del /q $(EXEC)
endif

## Targets not expecting files of the same name
.PHONY: all run clean fclean
