CC = mpicc
CFLAGS = -g -Wall -Wvla -Werror -Wno-error=unused-variable -Wno-error=unused-but-set-variable

LIBDIR = -L./
INCDIR = -I./

TARGET = bitonic

.PHONY: clean

all: $(TARGET)

$(TARGET): parallelbitonic.c
	$(CC) $(INDIRS) $(CFLAGS) -o $(TARGET) parallelbitonic.c $(LIBS) -lm


clean:
	$(RM) $(TARGET)
