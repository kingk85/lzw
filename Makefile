CC=g++
CFLAGS=-c
LDFLAGS=
SOURCES=main.cpp c.cpp C_c.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=compress

all: $(SOURCES) $(EXECUTABLE)
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@
.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
clean:
	rm -rf *o compress