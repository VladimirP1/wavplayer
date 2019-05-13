#pragma once
#include <alsa/asoundlib.h>
#include <alsaexception.hpp>

class AlsaPCM {
public:
    AlsaPCM(char const* name);
    ~AlsaPCM();

    void drain();
    void setParams(short sampleAlignment,
        	snd_pcm_format_t format,
        	unsigned int channels,
	        unsigned int rate,
        	unsigned int latencyUs);

    int write(void* data, size_t bytes);

private:
    snd_pcm_t *handle;
    short alignment = 0;

};
