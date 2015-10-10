#
# Makefile for skod.
# 
# Copyright (c) 2015 by Hypsurus <hypsurus@mail.ru>
#
# This file is part of skod project
#  
# Development version without install/uninstall
#

# Compiler and linker
# on gnu/linux it will be symlink to gcc
CC=cc

# Version
PACKAGE_VERSION=1.0

# C flags
CFLAGS = -ggdb -DPACKAGE_VERSION=\"${PACKAGE_VERSION}\" -Wall -Werror

# Sources
HEADERS_SOURCES=$(wildcard *.h)
SOURCES=$(wildcard *.c)
BIN=skod

all: skod

skod:
	$(CC) $(CFLAGS) $(SOURCES) -o $(BIN)

# Enable debug
debug: CFLAGS += -DDEBUG
debug: skod

clean:
	rm -f skod

.PHONY: all skod
