# Compiler and flags
CC := gcc
CFLAGS := -Wall -Wextra -O0 -g
LDFLAGS :=

# Directories
LIB_DIR := lib
IMG_PROC_DIR := image_processing
SRC_DIRS := . $(IMG_PROC_DIR)
LIB_SUBDIRS := $(shell find $(LIB_DIR) -type d)

# Include paths
INCLUDES := $(addprefix -I,$(LIB_SUBDIRS) $(IMG_PROC_DIR))

# Output
TARGET := main

# Source files
SRCS := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
LIB_SRCS := $(foreach dir,$(LIB_SUBDIRS),$(wildcard $(dir)/*.c))

# Object files
OBJS := $(SRCS:.c=.o)
LIB_OBJS := $(LIB_SRCS:.c=.o)

# All objects
ALL_OBJS := $(OBJS) $(LIB_OBJS)

# Dependency files
DEPS := $(ALL_OBJS:.o=.d)

# Silence commands
.SILENT:

# Default target
all: $(TARGET)

# Link
$(TARGET): $(ALL_OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

# Compile main sources with full warnings
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@

# Compile library sources without warnings
$(LIB_DIR)/%.o: $(LIB_DIR)/%.c
	$(CC) -w $(INCLUDES) -MMD -MP -c $< -o $@

# Run target
run: $(TARGET)
	./$(TARGET)

# Clean
clean:
	rm -f $(ALL_OBJS) $(DEPS) $(TARGET)

# Include dependencies
-include $(DEPS)

.PHONY: all run clean
