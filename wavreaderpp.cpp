#include "wavreaderpp.hh"
#include "wavreaderexception.hpp"
#include <exception>

WavReader::WavReader() {
    if (!wav_reader_alloc(&reader)) {
        throw std::bad_alloc();
    }
}

WavReader::~WavReader() noexcept {
    wav_reader_free(&reader);
}

void WavReader::open(const char *filename) {
    if (!wav_reader_open(reader, filename)) {
        throw WavReaderException {"Could not open file", reader->error};
    }
}

void WavReader::close() {
    wav_reader_close(reader);
}

int WavReader::getChannels() const {
    return reader->header.num_channels;
}

double WavReader::getDuration() const {
    return wav_reader_get_duration(reader);
}

short WavReader::getSampleAlignment() const {
    return reader->header.sample_alignment;
}

int WavReader::getSampleRate() const {
    return reader->header.sample_rate;
}

int WavReader::getByteRate() const {
    return reader->header.byte_rate;
}

snd_pcm_format_t WavReader::getFormat() const {
    snd_pcm_format_t fmt;

    if (!wav_reader_get_format(reader, &fmt)) {
        throw WavReaderException {"Unknown sample format", ENOSYS};
    }

    return fmt;
}

int WavReader::readFile(void* buf, size_t len) {
    int bytes = wav_reader_read(reader, (uint8_t*)buf, len);

    if(bytes == 0 && !reader->eof) {
        throw WavReaderException {"Could not read from file", reader->error};
    }

    return bytes;
}

void WavReader::loadFile(const char *filename, Watchdog* wdt) {
    if (fileLoaded) {
        throw WavReaderException {"A WavReader can load a file only once", ENOSYS};
    }

    fileLoaded = true;

    try {
        open(filename);

        samples.resize(reader->header.data_bytes);

        for (size_t pos = 0; (!reader->eof) && pos < samples.size(); ) {
            pos += readFile(&samples[pos], std::min(readerBlockSize, samples.size() - pos));
            if (wdt) {
                wdt->kick();
            }
        }

        close();

        getFormat();
    } catch(WavReaderException& e) {
        if (reader->file) {
            close();
        }
        samples.clear();
        samples.shrink_to_fit();

        throw;
    }
}

size_t WavReader::read(void *buf, size_t length) {
    auto start = samples.begin() + curByte;
    auto end = std::min(start + length, samples.end());

    std::copy(start, end, (uint8_t*)buf);

    size_t actuallyRead = std::distance(start, end);
    curByte += actuallyRead;

    return actuallyRead;
}

void WavReader::rewind() {
    curByte = 0;
}


