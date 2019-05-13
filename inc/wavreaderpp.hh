#pragma once

#include <wavreaderexception.hpp>
#include <wdt.hh>
extern "C" {
    #include <wavreader.h>
}
#include <vector>

class WavReader {
public:
    WavReader();
    ~WavReader() noexcept;

    void loadFile(char const* filename, Watchdog* wdt = nullptr);

    double getDuration() const;
    int getChannels() const;
    short getSampleAlignment() const;
    int getByteRate() const;
    snd_pcm_format_t getFormat() const;
    int getSampleRate() const;

    size_t read(void* buf, size_t length);
    void rewind();

private:
    const size_t readerBlockSize = 1024 * 1024;

    wav_reader* reader = nullptr;
    std::vector<uint8_t> samples;
    size_t curByte = 0;
    bool fileLoaded = false;

    int readFile(void* buf, size_t len);
    void open(char const* filename);
    void close();

    WavReader(const WavReader& reader) = delete;
    WavReader(WavReader&& reader) = delete;
    WavReader& operator=(const WavReader& reader) = delete;
    WavReader& operator=(WavReader&& reader) = delete;
};
