all: skin_detection

skin_detection: skin_detection.o
	g++ skin_detection.o -o skin_detection `pkg-config --libs opencv` 

skin_detection.o: skin_detection.cpp
	g++ -c skin_detection.cpp `pkg-config --cflags opencv` 
	@echo 'Finished building target: $@'

clean:
	rm -rf *o skin_detection
