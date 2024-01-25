# Define o nome do executável que será gerado
TARGET_EXEC := ordena

# Define o diretório onde os arquivos objeto serão gerados
BUILD_DIR := ./build

# Define o diretório onde os arquivos fonte estão localizados
SRC_DIRS := ./src

# Encontra todos os arquivos C++ nos diretórios de origem
SRCS := $(shell find $(SRC_DIRS) -name '*.c')

# Define os arquivos objeto que serão gerados a partir dos arquivos fonte
OBJS := $(patsubst $(SRC_DIRS)/%.c,$(BUILD_DIR)/src/%.o,$(SRCS))
OBJS_DEBUG := $(patsubst $(SRC_DIRS)/%.c,$(BUILD_DIR)/src/%.debug.o,$(SRCS))

# Define os arquivos de dependência para cada arquivo objeto
DEPS := $(OBJS:.o=.d)

# Encontra todos os diretórios de origem e adiciona a flag -I para cada um
INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I, $(INC_DIRS))

# Define as flags de compilação para o pré-processador
CFLAGS := $(INC_FLAGS) -MMD -MP -Wall -Wextra -Werror -std=c17
CFLAGS_DEBUG := $(CFLAGS) -g

# Define a regra para gerar o executável
# $(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
# 	$(CC) $(OBJS) -o $@ $(LDFLAGS)
$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)
# Define a regra para gerar o arquivo objeto de debug
$(BUILD_DIR)/$(TARGET_EXEC)_debug: $(OBJS_DEBUG)
	$(CC) $(OBJS_DEBUG) -o $@ $(LDFLAGS)

# Define a regra para gerar cada arquivo objeto a partir do arquivo fonte correspondente
$(BUILD_DIR)/src/%.o: $(SRC_DIRS)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS_DEBUG) $(CCFLAGS) -c $< -o $@

# Define a regra para gerar cada arquivo objeto de debug a partir do arquivo fonte correspondente
$(BUILD_DIR)/src/%.debug.o: $(SRC_DIRS)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS_DEBUG) $(CFLAGS) -c $< -o $@

# Define a regra para limpar os arquivos gerados
.PHONY: clean
clean:
	rm -r $(BUILD_DIR)
	rm -rf PROVAO.bin
	rm -rf ordena

.PHONY: valgrind
valgrind: $(BUILD_DIR)/$(TARGET_EXEC)_debug
	# valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file=valgrind-out.txt $(BUILD_DIR)/$(TARGET_EXEC)_debug


# Inclui os arquivos de dependência
-include $(DEPS)
