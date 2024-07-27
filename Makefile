CC	= gcc
CFLAGS	= -O2 
LIBS	= -lSDL2 -lSDL2_image -lSDL2_mixer

all:	soundIt.o lizardshi

soundIt.o:	
	$(CC) -c $(CFLAGS) soundIt.c 

lizardshi:
	$(CC) $(CFLAGS) -o lizardshi lizardshi.c svgalib_compat.c $(LIBS) soundIt.o

clean:
	rm -f *.o lizardshi
