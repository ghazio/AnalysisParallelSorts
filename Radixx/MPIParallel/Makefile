MPCC = mpicxx
CFLAGS = -g -Wall -Wvla -Werror -Wno-error=unused-variable -Wno-error=unused-but-set-variable

LIBDIR = -L/usr/local/stow/openmpi-3.1.3/lib
INCDIR = -I./
LIBS= $(LIBDIR) -lmpi

TARGET = radixMPIParallel

.PHONY: clean

all: $(TARGET)

$(TARGET): radixMPIParallel.cpp
	$(CC) $(INDIRS) $(CFLAGS) -o $(TARGET) radixMPIParallel.cpp $(LIBS) -lm -lstdc++


clean:
	$(RM) $(TARGET)
