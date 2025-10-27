CC=gcc

CFLAGS  := -Wall -Wextra -fsanitize=address -fsanitize=pointer-compare -fPIE
INCLUDE := -I../include
LIBS    := -lraylib -lm 

BUILD   := ../build
OBJECTS := $(BUILD)/drawop.o\
	$(BUILD)/main.o\
	$(BUILD)/drawopapi.o\
	$(BUILD)/internal.o

INSPREFIX := ~/.local

