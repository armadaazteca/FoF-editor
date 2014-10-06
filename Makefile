CC=g++
FLAGS_debug=-g -O0 -Wall -std=c++11 `wx-config --cppflags`
FLAGS_release=-O4 -std=c++11 `wx-config --cppflags`
OBJECTS=obj/$(CONF)/NewEditor.o obj/$(CONF)/MainWindow.o obj/$(CONF)/DatSprOpenSaveDialog.o \
        obj/$(CONF)/AboutDialog.o obj/$(CONF)/QuickGuideDialog.o obj/$(CONF)/DatSprReaderWriter.o \
        obj/$(CONF)/Settings.o obj/$(CONF)/Events.o obj/$(CONF)/Config.o obj/$(CONF)/Utils.o
RESOURCES=bin/$(CONF)/res/icons/green_arrow_left.png bin/$(CONF)/res/icons/green_arrow_top_left.png

all: bin/$(CONF)/NewEditor

bin/$(CONF)/NewEditor: $(OBJECTS) $(RESOURCES)
	$(CC) $(OBJECTS) -o bin/$(CONF)/NewEditor `wx-config --libs`

obj/$(CONF)/NewEditor.o: src/NewEditor.cpp src/NewEditor.h src/MainWindow.h src/Config.h
	$(CC) $(FLAGS_$(CONF)) -c src/NewEditor.cpp -o $@

obj/$(CONF)/MainWindow.o: src/MainWindow.cpp src/MainWindow.h src/DatSprOpenSaveDialog.h src/AboutDialog.h \
                          src/DatSprReaderWriter.h src/Events.h src/Config.h src/Utils.h
	$(CC) $(FLAGS_$(CONF)) -c src/MainWindow.cpp -o $@

obj/$(CONF)/DatSprOpenSaveDialog.o: src/DatSprOpenSaveDialog.cpp src/DatSprOpenSaveDialog.h src/Settings.h src/Interfaces.h
	$(CC) $(FLAGS_$(CONF)) -c src/DatSprOpenSaveDialog.cpp -o $@
	
obj/$(CONF)/AboutDialog.o: src/AboutDialog.cpp src/AboutDialog.h src/Config.h
	$(CC) $(FLAGS_$(CONF)) -c src/AboutDialog.cpp -o $@
	
obj/$(CONF)/QuickGuideDialog.o: src/QuickGuideDialog.cpp src/QuickGuideDialog.h src/Config.h
	$(CC) $(FLAGS_$(CONF)) -c src/QuickGuideDialog.cpp -o $@

obj/$(CONF)/DatSprReaderWriter.o: src/DatSprReaderWriter.cpp src/DatSprReaderWriter.h src/DatSprStructs.h src/Interfaces.h
	$(CC) $(FLAGS_$(CONF)) -c src/DatSprReaderWriter.cpp -o $@

obj/$(CONF)/Settings.o: src/Settings.cpp src/Settings.h
	$(CC) $(FLAGS_$(CONF)) -c src/Settings.cpp -o $@

obj/$(CONF)/Events.o: src/Events.cpp src/Events.h
	$(CC) $(FLAGS_$(CONF)) -c src/Events.cpp -o $@
	
obj/$(CONF)/Config.o: src/Config.cpp src/Config.h
	$(CC) $(FLAGS_$(CONF)) -c src/Config.cpp -o $@

obj/$(CONF)/Utils.o: src/Utils.cpp src/Utils.h
	$(CC) $(FLAGS_$(CONF)) -c src/Utils.cpp -o $@

bin/$(CONF)/res/icons/green_arrow_left.png: res/icons/green_arrow_left.png
	mkdir -p bin/$(CONF)/res/icons
	cp $^ $@

bin/$(CONF)/res/icons/green_arrow_top_left.png: res/icons/green_arrow_top_left.png
	mkdir -p bin/$(CONF)/res/icons
	cp $^ $@

.PHONY: clean
clean:
	rm -rf obj/$(CONF)/* bin/$(CONF)/*