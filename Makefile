CC = g++
CFLAGS = -Wall -g -c -std=c++11
CFLAGS += `pkg-config --cflags glfw3`

LDFLAGS = `pkg-config --static --libs glfw3`

SOURCES = vm.cpp renderer.cpp chip.cpp
OBJECTS = $(SOURCES:.cpp=.o)

OUTPUTDIR = bin

EXECUTABLE = vm

all: $(SOURCES) $(EXECUTABLE)
	 
$(EXECUTABLE): $(OBJECTS)
	mkdir -p $(OUTPUTDIR)	
	$(CC) $(OBJECTS) -o $(OUTPUTDIR)/$@ $(LDFLAGS)
	 
.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
	 
clean:
	rm $(OBJECTS) $(EXECUTABLE)
	

