CC = emcc
COMMON_CFLAGS = -O3
DEBUG_CFLAGS = $(COMMON_CFLAGS) -g
RELEASE_CFLAGS = $(COMMON_CFLAGS) -flto
LIBS = -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_SDL_MIXER=2

OUTPUT = lizardshi.html

SOURCES = lizardshi.c svgalib_compat.c soundIt.c
OBJECTS = $(SOURCES:.c=.o)

EMFLAGS = -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s ASYNCIFY=1 -s EXPORTED_RUNTIME_METHODS=['ccall','cwrap']
DEBUGFLAGS = -s ASSERTIONS=1 -s SAFE_HEAP=1 -s STACK_OVERFLOW_CHECK=1 -s DEMANGLE_SUPPORT=1
MAPFLAGS = -gsource-map --source-map-base http://localhost:8000/

RELEASEFLAGS = -s ENVIRONMENT=web -s NO_EXIT_RUNTIME=1 -s TEXTDECODER=2 -s ABORTING_MALLOC=0 --closure 1

PRELOAD = --preload-file bitout  --preload-file end.ub --preload-file font.dat --preload-file levelshi --preload-file grind.ub --preload-file ow.ub --preload-file scream.ub --preload-file slurp.ub --preload-file toc.ub --preload-file whine.ub --preload-file boom.ub --preload-file data --preload-file explode.ub --preload-file fontdata  --preload-file sloop.ub --preload-file splat.ub --preload-file click.ub --preload-file data32 --preload-file flush.ub --preload-file getye.ub   --preload-file mapsound.ub --preload-file roar.ub --preload-file sl.ub --preload-file textdata --preload-file wheep.ub

all: debug

debug: CFLAGS = $(DEBUG_CFLAGS)
debug: EXTRA_FLAGS = $(DEBUGFLAGS) $(MAPFLAGS)
debug: $(OUTPUT)

release: CFLAGS = $(RELEASE_CFLAGS)
release: EXTRA_FLAGS = $(RELEASEFLAGS)
release: clean $(OUTPUT)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OUTPUT): $(OBJECTS)
	$(CC) $(CFLAGS) $(EMFLAGS) $(EXTRA_FLAGS) $(OBJECTS) -o $@ $(LIBS) $(PRELOAD)

clean:
	rm -f $(OBJECTS) $(OUTPUT) $(OUTPUT:.html=.js) $(OUTPUT:.html=.wasm) $(OUTPUT:.html=.data) $(OUTPUT:.html=.wasm.map)

.PHONY: all debug release clean