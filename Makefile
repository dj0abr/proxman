# makefile to build proxman
# usage:
# make clean ... delete all build files
# make       ... create the application

CXXFLAGS = -Wall -O3 -std=c++0x -Wno-write-strings -Wno-narrowing -I /usr/include/freetype2 -I /usr/include/kmclib
LDFLAGS = -pthread -lpthread -lm -lkmclib -lfreetype -lrt 
OBJ = proxman.o proxmox_handler.o

default: $(OBJ)
	g++ $(CXXFLAGS) -o proxman $(OBJ) $(LDFLAGS)

clean:
	rm -rf *.o proxman
