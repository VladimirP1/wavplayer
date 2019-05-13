#include <math.h>
#include <stdio.h>
#include <wavreader.h>

bool wav_reader_alloc(wav_reader **reader) {
    (*reader) = (wav_reader*) malloc(sizeof(wav_reader));
    if (! *reader) {
        return false;
    }

    (*reader)->file = 0;
    memset(&(*reader)->header, 0, sizeof((*reader)->header));
    (*reader)->eof = 0;
    (*reader)->error = 0;
    (*reader)->data_bytes_read = 0;
    return true;
}

void wav_reader_free(wav_reader **reader) {
    if ((*reader)->file) {
        fclose((*reader)->file);
    }
    free(*reader);
}

bool wav_reader_open(wav_reader* self, const char* filename) {
    self->file = fopen(filename, "r");
    if (self->file == NULL) {
        self->error = errno;
        return false;
    }

    int nread = fread(&self->header, sizeof(wav_header), 1, self->file);
    if (nread != 1) {
        self->error = errno;
        return false;
    }

    if (strncmp(self->header.riff_header, "RIFF", 4) != 0) {
        self->error = EINVAL;
        return false;
    }

    if (strncmp(self->header.wave_header, "WAVE", 4) != 0) {
        self->error = EINVAL;
        return false;
    }

    if (strncmp(self->header.fmt_header, "fmt ", 4) != 0) {
        self->error = EINVAL;
        return false;
    }

    // We support PCM only
    if (self->header.fmt_chunk_size != 16) {
        self->error = ENOSYS;
        return false;
    }

    if(self->header.audio_format != 1) {
        self->error = ENOSYS;
        return false;
    }

    // Sainty check
    if (self->header.byte_rate <= 0) {
        self->error = EINVAL;
        return false;
    }

    // Workaround buggy files
    self->header.data_bytes = self->header.wav_size + 8 - sizeof(wav_header);

    return true;
}

void wav_reader_close(wav_reader* self) {
    fclose(self->file);
    self->file = NULL;
}

double wav_reader_get_duration(wav_reader* self) {
    return (double)self->header.data_bytes / (double)self->header.byte_rate;
}

double wav_reader_get_cur_pos(wav_reader* self) {
    return (double)self->data_bytes_read / (double)self->header.byte_rate;
}

bool wav_reader_get_format(wav_reader* self, snd_pcm_format_t* e) {
    switch(self->header.bit_depth) {
        case 8:
            (*e) = SND_PCM_FORMAT_U8;
            return true;
        case 16:
            (*e) = SND_PCM_FORMAT_S16_LE;
            return true;
        case 24:
            (*e) = SND_PCM_FORMAT_S24_3LE;
            return true;
        case 32:
            (*e) = SND_PCM_FORMAT_S32_LE;
            return true;
        default:
            self->error = ENOSYS;
            return false;
    }
}

int wav_reader_read(wav_reader* self, uint8_t* buf, size_t len) {
    size_t nread;
    for (nread = 0; nread < len; ) {
        nread += fread(buf + nread, 1, len - nread, self->file);
        if (feof(self->file)) {
            self->eof = true;
            return 0;
        }
        if (ferror(self->file)) {
            self->error = errno;
            return 0;
        }
    }

    self->data_bytes_read += nread;
    return nread;
}
