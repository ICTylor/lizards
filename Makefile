CC = emcc
#CFLAGS = -O0 -g
CFLAGS = -O3 -g
LIBS = -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_SDL_MIXER=2

OUTPUT = lizardshi.html

SOURCES = lizardshi.c svgalib_compat.c soundIt.c
OBJECTS = $(SOURCES:.c=.o)

EMFLAGS = -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s EXPORTED_RUNTIME_METHODS=['ccall','cwrap']
DEBUGFLAGS = -s ASSERTIONS=1 -s SAFE_HEAP=1 -s STACK_OVERFLOW_CHECK=1 -s DEMANGLE_SUPPORT=1
MAPFLAGS = -gsource-map --source-map-base http://localhost:8000/

PRELOAD = --preload-file bitout  --preload-file end.ub --preload-file font.dat --preload-file levelshi --preload-file grind.ub --preload-file ow.ub --preload-file scream.ub --preload-file slurp.ub --preload-file toc.ub --preload-file whine.ub --preload-file boom.ub --preload-file data --preload-file explode.ub --preload-file fontdata  --preload-file sloop.ub --preload-file splat.ub --preload-file click.ub --preload-file data32 --preload-file flush.ub --preload-file getye.ub   --preload-file mapsound.ub --preload-file roar.ub --preload-file sl.ub --preload-file textdata --preload-file wheep.ub

all: $(OUTPUT)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OUTPUT): $(OBJECTS)
	$(CC) $(CFLAGS) $(EMFLAGS) $(DEBUGFLAGS) $(MAPFLAGS) $(OBJECTS) -o $@ $(LIBS) $(PRELOAD)

clean:
	rm -f $(OBJECTS) $(OUTPUT) $(OUTPUT:.html=.js) $(OUTPUT:.html=.wasm) $(OUTPUT:.html=.data) $(OUTPUT:.html=.wasm.map)

.PHONY: all clean
#
#.PHONY: all clean
#
#CC = emcc
#CFLAGS = -O0 -g
#LIBS = -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_SDL_MIXER=2
#
#OUTPUT = lizardshi.html
#
#SOURCES = lizardshi.c svgalib_compat.c soundIt.c
#OBJECTS = $(SOURCES:.c=.o)
#
#EMFLAGS = -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s EXPORTED_RUNTIME_METHODS=['ccall','cwrap']
#DEBUGFLAGS = -s ASSERTIONS=1 -s SAFE_HEAP=1 -s STACK_OVERFLOW_CHECK=1 -s DEMANGLE_SUPPORT=1
#
##   PRELOAD = --preload-file ./@/ --no-heap-copy --exclude *.c --exclude *.h --exclude *.o --exclude $(OUTPUT) --exclude *.js --exclude *.wasm --exclude Makefile
#PRELOAD = --preload-file bitout  --preload-file end.ub --preload-file font.dat --preload-file grind.ub --preload-file ow.ub --preload-file scream.ub --preload-file slurp.ub --preload-file toc.ub --preload-file whine.ub --preload-file boom.ub --preload-file data --preload-file explode.ub --preload-file fontdata  --preload-file sloop.ub --preload-file splat.ub --preload-file click.ub --preload-file data32 --preload-file flush.ub --preload-file getye.ub   --preload-file mapsound.ub --preload-file roar.ub --preload-file sl.ub --preload-file textdata --preload-file wheep.ub
#
#all: $(OUTPUT)
#
#%.o: %.c
#	$(CC) $(CFLAGS) -c $< -o $@
#
#$(OUTPUT): $(OBJECTS)
#	$(CC) $(CFLAGS) $(EMFLAGS) $(DEBUGFLAGS) $(OBJECTS) -o $@ $(LIBS) $(PRELOAD) --source-map-base http://localhost:8000/
#
#clean:
#	rm -f $(OBJECTS) $(OUTPUT) $(OUTPUT:.html=.js) $(OUTPUT:.html=.wasm) $(OUTPUT:.html=.data) $(OUTPUT:.html=.wasm.map)