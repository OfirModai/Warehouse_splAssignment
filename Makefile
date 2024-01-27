# Makefile

CC = g++
CFLAGS = -Wall -Weffc++ -std=c++11 -g -Iinclude
TARGET = bin/warehouse
SRCDIR = src
INCLUDEDIR = include
BUILDDIR = bin
SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean

clean:
	rm -rf $(BUILDDIR) $(TARGET)
