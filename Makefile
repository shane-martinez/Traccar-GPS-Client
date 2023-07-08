# Makefile for the project

# Compiler
CC = g++

# Compiler flags
CFLAGS = -std=c++17 -Wall -I.

# Linker flags
LDFLAGS = -lcurl -lpthread

# Source files
SRCS = $(wildcard *.cpp) $(wildcard GPS/*.cpp) $(wildcard GPS/TinyGPS/*.cpp)

# Object files directory
OBJDIR = .build
OBJ_SUBDIRS = $(sort $(dir $(OBJS)))

# Object files
OBJS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(SRCS))
DEPS = $(patsubst %.o,%.d,$(OBJS))

# Executable name
EXEC = gps_trac

# Default target
all: $(EXEC)

# Create the object files directory
$(shell mkdir -p $(OBJ_SUBDIRS))

# Include the dependency files
-include $(DEPS)

# Rule for making the target
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Rule for making the object files
$(OBJDIR)/%.o: %.cpp
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@ -MF $(@:.o=.d)

# Clean target
.PHONY: clean
clean:
	rm -rf $(OBJDIR) $(EXEC)
