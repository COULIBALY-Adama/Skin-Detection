all: detection_peau

detection_peau: detection_peau.o
	g++ detection_peau.o -o detection_peau `pkg-config --libs opencv` 

detection_peau.o: detection_peau.cpp
	g++ -c detection_peau.cpp `pkg-config --cflags opencv` 
	@echo 'Finished building target: $@'

clean:
	rm -rf *o detection_peau
