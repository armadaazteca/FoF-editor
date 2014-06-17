CC=g++
FLAGS=-g -Wall -std=c++11 `wx-config --cppflags`
OBJECTS=obj/$(CONF)/NewEditor.o obj/$(CONF)/MainWindow.o obj/$(CONF)/DatSprOpenDialog.o \
        obj/$(CONF)/DatSprReaderWriter.o obj/$(CONF)/Settings.o obj/$(CONF)/Events.o
RESOURCES=bin/$(CONF)/res/icons/green_arrow_left.png bin/$(CONF)/res/misc/sprite_placeholder.png

all: bin/$(CONF)/NewEditor

bin/$(CONF)/NewEditor: $(OBJECTS) $(RESOURCES)
	$(CC) $(OBJECTS) -o bin/$(CONF)/NewEditor `wx-config --libs`
	
obj/$(CONF)/NewEditor.o: src/NewEditor.cpp src/NewEditor.h src/MainWindow.h 
	$(CC) $(FLAGS) -c src/NewEditor.cpp -o $@

obj/$(CONF)/MainWindow.o: src/MainWindow.cpp src/MainWindow.h src/DatSprOpenDialog.h src/DatSprReaderWriter.h src/Events.h
	$(CC) $(FLAGS) -c src/MainWindow.cpp -o $@

obj/$(CONF)/DatSprOpenDialog.o: src/DatSprOpenDialog.cpp src/DatSprOpenDialog.h src/Settings.h src/Interfaces.h
	$(CC) $(FLAGS) -c src/DatSprOpenDialog.cpp -o $@
	
obj/$(CONF)/DatSprReaderWriter.o: src/DatSprReaderWriter.cpp src/DatSprReaderWriter.h src/DatSprStructs.h src/Interfaces.h
	$(CC) $(FLAGS) -c src/DatSprReaderWriter.cpp -o $@
	
obj/$(CONF)/Settings.o: src/Settings.cpp src/Settings.h
	$(CC) $(FLAGS) -c src/Settings.cpp -o $@
	
obj/$(CONF)/Events.o: src/Events.cpp src/Events.h
	$(CC) $(FLAGS) -c src/Events.cpp -o $@
	
bin/$(CONF)/res/icons/green_arrow_left.png: res/icons/green_arrow_left.png
	mkdir -p bin/$(CONF)/res/icons
	cp $^ $@
	
bin/$(CONF)/res/misc/sprite_placeholder.png: res/misc/sprite_placeholder.png
	mkdir -p bin/$(CONF)/res/misc
	cp $^ $@
	
.PHONY: clean
clean:
	rm -rf obj/$(CONF)/* bin/$(CONF)/*