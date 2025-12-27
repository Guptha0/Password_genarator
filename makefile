# SecurePassGen - Makefile
# Build system for cross-platform password generator

# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11 -D_POSIX_C_SOURCE=200809L
LDFLAGS = 
LIBS = -lm

# Platform detection
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    CFLAGS += -D_LINUX
    LIBS += -lpthread
endif
ifeq ($(UNAME_S),Darwin)
    CFLAGS += -D_MACOS
    LIBS += -framework Carbon -framework Cocoa
endif
ifeq ($(OS),Windows_NT)
    CFLAGS += -D_WIN32 -D_WINDOWS
    LIBS += -luser32 -lkernel32 -lgdi32
endif

# Directories
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

# Source files
SRCS = $(SRC_DIR)/main.c \
       $(SRC_DIR)/password.c \
       $(SRC_DIR)/security.c \
       $(SRC_DIR)/ui.c \
       $(SRC_DIR)/clipboard.c \
       $(SRC_DIR)/utils.c \
       $(SRC_DIR)/file_ops.c

# Object files
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

# Executable name
TARGET = $(BIN_DIR)/passgen
TARGET_WINDOWS = $(BIN_DIR)/passgen.exe

# Default target
all: $(TARGET)

# Create directories
$(BUILD_DIR) $(BIN_DIR):
	mkdir -p $@

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Link executable
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(LIBS)

# Windows executable (cross-compilation option)
windows: $(TARGET_WINDOWS)

$(TARGET_WINDOWS): $(SRCS)
	x86_64-w64-mingw32-gcc $(CFLAGS) -D_WIN32 $^ -o $@ $(LDFLAGS) -luser32 -lkernel32 -lgdi32

# Debug build
debug: CFLAGS += -g -DDEBUG -fsanitize=address -fsanitize=undefined
debug: clean all

# Release build with optimizations
release: CFLAGS += -O3 -flto -DNDEBUG
release: clean all

# Static linking
static: CFLAGS += -static
static: clean all

# Install (Linux/macOS)
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/passgen
	chmod +x /usr/local/bin/passgen
	@echo "Installed to /usr/local/bin/passgen"

# Uninstall
uninstall:
	rm -f /usr/local/bin/passgen
	@echo "Uninstalled from /usr/local/bin/passgen"

# Clean build files
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Run tests
test: all
	@echo "Running tests..."
	./$(TARGET) --version
	./$(TARGET) -l 16
	./$(TARGET) -l 24 -c 3
	@echo "Tests completed."

# Run with valgrind (Linux)
valgrind: debug
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET) -l 16

# Build documentation (requires Doxygen)
docs:
	doxygen Doxyfile

# Create distribution package
dist: clean
	mkdir -p dist/SecurePassGen
	cp -r $(SRC_DIR) Makefile README.md LICENSE CHANGELOG.md dist/SecurePassGen/
	tar -czf SecurePassGen-$(shell date +%Y%m%d).tar.gz -C dist SecurePassGen
	rm -rf dist
	@echo "Distribution package created: SecurePassGen-$(shell date +%Y%m%d).tar.gz"

# Help target
help:
	@echo "SecurePassGen Build System"
	@echo ""
	@echo "Targets:"
	@echo "  all       - Build the program (default)"
	@echo "  debug     - Build with debug symbols and sanitizers"
	@echo "  release   - Build with maximum optimizations"
	@echo "  static    - Build statically linked executable"
	@echo "  windows   - Cross-compile for Windows"
	@echo "  install   - Install to /usr/local/bin"
	@echo "  uninstall - Uninstall from /usr/local/bin"
	@echo "  clean     - Remove build files"
	@echo "  test      - Run basic tests"
	@echo "  valgrind  - Run with valgrind (Linux)"
	@echo "  docs      - Generate documentation"
	@echo "  dist      - Create distribution package"
	@echo "  help      - Show this help message"

# Phony targets
.PHONY: all debug release static windows install uninstall clean test valgrind docs dist help