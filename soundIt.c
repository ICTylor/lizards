/* SoundIt library 0.03 - SDL2 Port (Debug Version)

   Original Copyright 1994 Brad Pitzel  pitzel@cs.sfu.ca
   SDL2 Port Copyright 2024 [Your Name]

   Feel free to use/distribute/modify as long as proper credits
   are included.
*/

#ifndef SOUNDIT_VERS
#define SOUNDIT_VERS "0.03-SDL2-DEBUG"

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_CHANNELS 32
#define DEBUG_LOG(msg, ...) fprintf(stderr, "DEBUG: " msg "\n", ##__VA_ARGS__)

struct Sample {
    Mix_Chunk *chunk;
    int len;
};

typedef struct Sample Sample;

static const Sample *S_sounds = NULL;
static int S_num_sounds = 0;
static int S_num_channels = 6;
static int S_playback_freq = 44100;

int Snd_loadRawSample(const char *file, Sample *sample) {
    SDL_RWops *rw = SDL_RWFromFile(file, "rb");
    if (!rw) {
        DEBUG_LOG("Failed to open file: %s", SDL_GetError());
        return -1;
    }

    Sint64 file_size = SDL_RWsize(rw);
    if (file_size < 0) {
        DEBUG_LOG("Failed to get file size: %s", SDL_GetError());
        SDL_RWclose(rw);
        return -1;
    }

    Uint8 *buffer = (Uint8 *)malloc(file_size);
    if (!buffer) {
        DEBUG_LOG("Failed to allocate memory for buffer");
        SDL_RWclose(rw);
        return -2;
    }

    Sint64 read_bytes = SDL_RWread(rw, buffer, 1, file_size);
    SDL_RWclose(rw);

    if (read_bytes != file_size) {
        DEBUG_LOG("Failed to read entire file: read %d bytes, expected %d", (int)read_bytes, (int)file_size);
        free(buffer);
        return -1;
    }

    // Attempt to load directly as WAV
    SDL_RWops *mem_rw = SDL_RWFromMem(buffer, file_size);
    if (!mem_rw) {
        DEBUG_LOG("Failed to create RWops from memory: %s", SDL_GetError());
        free(buffer);
        return -1;
    }

    sample->chunk = Mix_LoadWAV_RW(mem_rw, 1);
    if (!sample->chunk) {
        DEBUG_LOG("Failed to load WAV: %s", Mix_GetError());

        // If direct loading fails, try creating a WAV header
        DEBUG_LOG("Attempting to create WAV header...");

        Uint8 wav_header[44] = {
                'R', 'I', 'F', 'F',
                (Uint8)(file_size + 36), (Uint8)((file_size + 36) >> 8), (Uint8)((file_size + 36) >> 16), (Uint8)((file_size + 36) >> 24),
                'W', 'A', 'V', 'E',
                'f', 'm', 't', ' ',
                16, 0, 0, 0,
                1, 0,  // PCM format
                1, 0,  // Mono
                (Uint8)(S_playback_freq), (Uint8)(S_playback_freq >> 8), (Uint8)(S_playback_freq >> 16), (Uint8)(S_playback_freq >> 24),
                (Uint8)(S_playback_freq), (Uint8)(S_playback_freq >> 8), (Uint8)(S_playback_freq >> 16), (Uint8)(S_playback_freq >> 24),
                1, 0,  // Block align
                8, 0,  // Bits per sample
                'd', 'a', 't', 'a',
                (Uint8)(file_size), (Uint8)(file_size >> 8), (Uint8)(file_size >> 16), (Uint8)(file_size >> 24)
        };

        Uint8 *wav_buffer = (Uint8 *)malloc(sizeof(wav_header) + file_size);
        if (!wav_buffer) {
            DEBUG_LOG("Failed to allocate memory for WAV buffer");
            free(buffer);
            return -2;
        }

        memcpy(wav_buffer, wav_header, sizeof(wav_header));
        memcpy(wav_buffer + sizeof(wav_header), buffer, file_size);

        SDL_RWops *wav_rw = SDL_RWFromMem(wav_buffer, sizeof(wav_header) + file_size);
        if (!wav_rw) {
            DEBUG_LOG("Failed to create RWops for WAV: %s", SDL_GetError());
            free(buffer);
            free(wav_buffer);
            return -1;
        }

        sample->chunk = Mix_LoadWAV_RW(wav_rw, 1);
        if (!sample->chunk) {
            DEBUG_LOG("Failed to load WAV with header: %s", Mix_GetError());
            free(buffer);
            free(wav_buffer);
            return -1;
        }

        free(wav_buffer);
    }

    sample->len = file_size;
    free(buffer);
    DEBUG_LOG("Successfully loaded sample, length: %d", sample->len);
    return 0;
}

int Snd_init(int num_snd, const Sample *sa, int freq, int channels, const char *sound_device) {
    (void)sound_device; // Unused in SDL2 version

    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        return EXIT_FAILURE;
    }

    if (Mix_OpenAudio(freq, AUDIO_U8, 1, 1024) < 0) {
        SDL_Quit();
        return EXIT_FAILURE;
    }

    S_num_sounds = num_snd;
    S_sounds = sa;
    S_num_channels = channels;
    S_playback_freq = freq;

    Mix_AllocateChannels(channels);

    return EXIT_SUCCESS;
}

int Snd_restore() {
    Mix_CloseAudio();
    SDL_Quit();
    return EXIT_SUCCESS;
}

int Snd_effect(int sound_num, int channel) {
    if (sound_num < 0 || sound_num >= S_num_sounds || channel < 0 || channel >= S_num_channels) {
        return EXIT_FAILURE;
    }

    if (S_sounds[sound_num].chunk != NULL) {
        Mix_PlayChannel(channel, S_sounds[sound_num].chunk, 0);
        return EXIT_SUCCESS;
    } else {
        fprintf(stderr, "Referencing NULL sound entry\n");
        return EXIT_FAILURE;
    }
}

// Optional: Implement these functions if needed
/*
void Snd_reset(enum snd_channel channel) {
    Mix_HaltChannel(channel);
}

void Snd_reset() {
    Mix_HaltChannel(-1);
}
*/

#endif // SOUNDIT_VERS