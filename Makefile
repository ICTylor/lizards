CC = gcc
EMCC = emcc
CFLAGS = -Wall -I/usr/include/SDL2
RELEASE_CFLAGS = -O3 $(COMMON_CFLAGS)
EMFLAGS = -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s ASYNCIFY=1 -s EXPORTED_RUNTIME_METHODS=['ccall','cwrap'] --shell-file sdl2_template.html
LDFLAGS = -lSDL2 -lSDL2_image -lSDL2_mixer
DEBUGFLAGS = -O0 -g -s ASSERTIONS=1 -s SAFE_HEAP=1 -s STACK_OVERFLOW_CHECK=1 -s DEMANGLE_SUPPORT=1
RELEASEFLAGS = -s ENVIRONMENT=web -s NO_EXIT_RUNTIME=1 -s TEXTDECODER=2 -s ABORTING_MALLOC=0
MAPFLAGS = -gsource-map --source-map-base http://localhost:8000/

SRCS = lizardshi.c svgalib_compat.c soundIt.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = lizardshi
EM_TARGET = lizardshi.html
PRELOAD = --preload-file bitout  --preload-file end.ub --preload-file font.dat --preload-file levelshi --preload-file grind.ub --preload-file ow.ub --preload-file scream.ub --preload-file slurp.ub --preload-file toc.ub --preload-file whine.ub --preload-file boom.ub --preload-file data --preload-file explode.ub --preload-file fontdata  --preload-file sloop.ub --preload-file splat.ub --preload-file click.ub --preload-file data32 --preload-file flush.ub --preload-file getye.ub   --preload-file mapsound.ub --preload-file roar.ub --preload-file sl.ub --preload-file textdata --preload-file wheep.ub

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

emscripten: $(SRCS)
	$(EMCC) $(CFLAGS) $(EMFLAGS) $(DEBUGFLAGS) $(SRCS) $(OBJECTS) -o $(EM_TARGET) -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_SDL_MIXER=2 $(PRELOAD)

emscripten_release: $(SRCS)
	$(EMCC) $(CFLAGS) $(RELEASE_CFLAGS) $(EMFLAGS) $(RELEASEFLAGS) $(SRCS) $(OBJECTS) -o $(EM_TARGET) -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_SDL_MIXER=2 $(PRELOAD)

clean:
	rm -f $(TARGET) $(EM_TARGET) $(EM_TARGET).js $(EM_TARGET).wasm

.PHONY: all clean emscripten