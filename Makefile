# Makefile for the project

# Compiler
CC = g++

# Compiler flags
CFLAGS = -std=c++17 -Wall -I.

# Linker flags
LDFLAGS = -lcurl -lpthread

# Source files
SRCS = $(wildcard *.cpp) $(wildcard GPS/*.cpp) $(wildcard GPS/TinyGPS/*.cpp)

# Object files
OBJS = $(SRCS:.cpp=.o)

# Dependencies
DEPS = $(OBJS:.o=.d)

# Executable name
EXEC = gps_trac

# Default target
all: $(EXEC)

# Include the dependency files
-include $(DEPS)

# Rule for making the target
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Rule for making the object files
%.o: %.cpp
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# Clean target
.PHONY: clean
clean:
	rm -f $(OBJS) $(EXEC) $(DEPS)
