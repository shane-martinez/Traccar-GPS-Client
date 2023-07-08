# Makefile for the project

# Compiler
CC = g++

# Compiler flags
CFLAGS = -std=c++17 -Wall -I./src

# Linker flags
LDFLAGS = -lcurl -lpthread

# Source files
SRCS = $(wildcard src/*.cpp) $(wildcard src/GPS/*.cpp) $(wildcard src/GPS/TinyGPS/*.cpp)

# Object files directory
OBJDIR = .build
OBJ_SUBDIRS = $(sort $(dir $(OBJS)))

# Object files
OBJS = $(patsubst src/%.cpp,$(OBJDIR)/%.o,$(SRCS))
DEPS = $(patsubst %.o,%.d,$(OBJS))

# Executable directory
BINDIR = bin

# Executable name
EXEC = $(BINDIR)/gps_trac

# Default target
all: $(EXEC)

# Create the object files directory
$(shell mkdir -p $(OBJ_SUBDIRS) $(BINDIR))

# Include the dependency files
-include $(DEPS)

# Rule for making the target
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Rule for making the object files
$(OBJDIR)/%.o: src/%.cpp
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@ -MF $(@:.o=.d)

# Clean target
.PHONY: clean
clean:
	rm -rf $(OBJDIR) $(BINDIR)
