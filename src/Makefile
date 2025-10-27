include config.mk

all: $(BUILD) $(OBJECTS) $(BUILD)/drawop


$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/main.o: main.c 		
	$(CC) $(CFLAGS) $(INCLUDE) -c main.c      -o $(BUILD)/main.o
$(BUILD)/drawop.o: drawop.c 		
	$(CC) $(CFLAGS) $(INCLUDE) -c drawop.c    -o $(BUILD)/drawop.o
$(BUILD)/drawopapi.o: drawopapi.c 	
	$(CC) $(CFLAGS) $(INCLUDE) -c drawopapi.c -o $(BUILD)/drawopapi.o
$(BUILD)/internal.o: internal.c 	
	$(CC) $(CFLAGS) $(INCLUDE) -c internal.c  -o $(BUILD)/internal.o -Wno-unused-parameter

$(BUILD)/drawop: $(OBJECTS) 		
	$(CC) $(CFLAGS) $(INCLUDE) $(OBJECTS) $(LIBS) -o $(BUILD)/drawop

install: $(BUILD)/drawop
	cp $(BUILD)/drawop $(INSPREFIX)/bin/drawop

clean:
	rm $(OBJECTS) $(BUILD)/drawop

.PHONY: clean all

