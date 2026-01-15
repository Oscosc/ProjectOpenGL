# ==============================
# Compiler et flags
# ==============================

CXX = g++
CC  = gcc
LDFLAGS = -lglfw -ldl -g -lm -fopenmp
COMPFLAGS = -fopenmp -Iincludes -MMD -MP -fdiagnostics-color=always


# ==============================
# Dossiers
# ==============================

SRC_DIR = src
OBJ_DIR = obj
TARGET  = igai_exe


# ==============================
# Fichiers source et objets
# ==============================

SRC_FILES = $(shell find $(SRC_DIR) -name "*.cpp")
C_SRC_FILES = $(shell find $(SRC_DIR) -name "*.c")

EXT_DIR   = includes/extern
EXT_FILES = $(shell find $(EXT_DIR) -name "*.cpp")

OBJ_FILES_SRC = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC_FILES))
OBJ_FILES_C   = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(C_SRC_FILES))
OBJ_FILES_EXT = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(EXT_FILES))
OBJ_FILES = $(OBJ_FILES_SRC) $(OBJ_FILES_C) $(OBJ_FILES_EXT)


# ==============================
# Règles
# ==============================

all: $(TARGET)

# Build de l’exécutable (sans afficher la ligne de commande)
$(TARGET): $(OBJ_FILES)
	@echo "\033[0;35m[LINKS] '$@' executable linked\033[0m"
	@$(CXX) $^ -o $@ $(LDFLAGS)
	@echo "\033[0;32m[BUILD] Build finished successfully !\033[0m"

# Compilation des .cpp
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "\033[0;34m[BUILD] Compiled $<\033[0m"
	@$(CXX) $(COMPFLAGS) -c $< -o $@

# Compilation des .cpp externes
$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo "\033[0;34m[BUILD] Compiled $<\033[0m"
	@$(CXX) $(COMPFLAGS) -c $< -o $@

# Compilation des .c
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "\033[0;34m[BUILD] Compiled $<\033[0m"
	@$(CC) -c $< -o $@

# Inclure les fichiers .d pour dépendances automatiques
-include $(OBJ_FILES:.o=.d)

# Nettoyage
clean:
	@echo "\033[0;31m[CLEAN] Cleaning objects and executable\033[0m"
	@rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all clean
