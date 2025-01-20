#Makefile.



BIN1 = build/drawop
BIN1.SRC = src/*.c
OPT_LEVEL = 3
INCLUDE = include
LIBS=raylib m
LINK_FLAGS=-L lib/
FLAGS=#-pg


include external/Makefile
EXTERNAL := ./external
include Makefile.mk
