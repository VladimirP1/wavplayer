#include <alsa.hh>

AlsaPCM::AlsaPCM(char const* name) {
    int err;
    err = snd_pcm_open(&handle, name, SND_PCM_STREAM_PLAYBACK, 0);

    if (err < 0) {
        throw AlsaException("Could not open PCM", err, true);
    }
}

AlsaPCM::~AlsaPCM() {
    snd_pcm_close(handle);
}

void AlsaPCM::drain() {
    snd_pcm_drain(handle);
}

void AlsaPCM::setParams(
        short sampleAlignment,
        snd_pcm_format_t format,
        unsigned int channels,
        unsigned int rate,
        unsigned int latencyUs)
{
    alignment = sampleAlignment;

    int err = snd_pcm_set_params(handle, format, SND_PCM_ACCESS_RW_INTERLEAVED, channels, rate, true, latencyUs);

    if(err < 0) {
        throw AlsaException("Could not set PCM params", err, true);
    }

}

int AlsaPCM::write(void *data, size_t bytes) {
    if (bytes % alignment != 0) {
        throw AlsaException("Length should be divisible by alignment", EINVAL, true);
    }

    int triedRecover = 0;

    auto frames = bytes / alignment;

    auto written = snd_pcm_writei(handle, data, frames);

    if (written < 0) {
        triedRecover = written;
        written = snd_pcm_recover(handle, frames, 0);
    }

    if (written < 0) {
        throw AlsaException("Could not recover PCM", written, true);
    }

    if(triedRecover) {
        throw AlsaException("Underrun occured", triedRecover, false);
    }

    return alignment * written;
}
