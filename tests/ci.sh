#!/bin/sh

CC=gcc   make -B
CC=clang make -B

# tcc segfaults due to atexit()?
# CC=tcc   make -B
