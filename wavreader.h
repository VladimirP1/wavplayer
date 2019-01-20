#pragma once
#include <alsa/asoundlib.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct wav_header {
    // RIFF Header
    char riff_header[4]; // Contains "RIFF"
    int wav_size; // Size of the wav portion of the file, which follows the first 8 bytes. File size - 8
    char wave_header[4]; // Contains "WAVE"

    // Format Header
    char fmt_header[4]; // Contains "fmt " (includes trailing space)
    int fmt_chunk_size; // Should be 16 for PCM
    short audio_format; // Should be 1 for PCM. 3 for IEEE Float
    short num_channels;
    int sample_rate;
    int byte_rate; // Number of bytes per second. sample_rate * num_channels * Bytes Per Sample
    short sample_alignment; // num_channels * Bytes Per Sample
    short bit_depth; // Number of bits per sample

    // Data
    char data_header[4]; // Contains "data"
    int data_bytes; // Number of bytes in data. Number of samples * num_channels * sample byte size
    // uint8_t bytes[]; // Remainder of wave file is bytes
} wav_header;

typedef struct wav_reader {
    FILE* file;
    wav_header header;
    bool eof;
    int error;
    size_t data_bytes_read;
} wav_reader;

extern bool wav_reader_alloc(wav_reader **reader);
extern void wav_reader_free(wav_reader **reader);
extern bool wav_reader_open(wav_reader* self, const char* filename);
extern void wav_reader_close(wav_reader* self);
extern double wav_reader_get_duration(wav_reader* self);
extern double wav_reader_get_cur_pos(wav_reader* self);
extern bool wav_reader_get_format(wav_reader* self, snd_pcm_format_t* e);
extern int wav_reader_read(wav_reader* self, uint8_t* buf, size_t len);
