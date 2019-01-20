#include <libgen.h>
#include <signal.h>

#include <wavreaderpp.hh>
#include <alsa.hh>
#include <wdt.hh>

#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

//#define USE_WDT

volatile bool stop = false;

class Player {
public:
    Player(std::string file, std::string prefix, std::ostream& log) : pcm("default"),
#ifdef USE_WDT
    wdt("/dev/watchdog"),
#endif
    log(log)
     {
        log << "Opening files ..." << std::endl;
        // These are fatal. Ignore exceptions
        std::cout << prefix << std::endl;
        errorNoFile.loadFile((prefix + std::string("/share/player/error_no_file.wav")).c_str());
        errorBadFile.loadFile((prefix + std::string("/share/player/error_bad_file.wav")).c_str());
        setupForReader(errorNoFile);
        setupForReader(errorBadFile);

        try {
            music.loadFile(file.c_str()
            #ifdef USE_WDT
            , &wdt
            #endif
            );
        } catch(WavReaderException& e) {
            if (e.getErrno() == EINVAL || e.getErrno() == ENOSYS) {
                badFile();
            }else {
                noFile();
            }
            throw;
        }

        try {
            setupForReader(music);
        } catch(AlsaException& e) {
            for(int i = 0; i < 10; i++) {
                play(errorBadFile);
            }
            throw;
        }
        log << "done!" << std::endl;
    }

    ~Player() {

    }

    void run() {
        while(true) {
            play(music);
            if(stop) {
            #ifdef USE_WDT
                wdt.magic();
            #endif
                break;
            }
        }
    }

private:
    AlsaPCM pcm;
#ifdef USE_WDT
    Watchdog wdt;
#endif

    WavReader music;
    WavReader errorNoFile;
    WavReader errorBadFile;

    std::vector<uint8_t> buffer;

    std::ostream& log;

    void setupForReader(WavReader const & reader) {
        pcm.setParams(reader.getSampleAlignment(),
            reader.getFormat(),
            reader.getChannels(),
            reader.getSampleRate(),
            500000
        );

        int bufsize = reader.getByteRate() / 8; // 1/8 of a second
        bufsize -= bufsize % reader.getSampleAlignment();

        buffer.resize(bufsize);
    }

    void play(WavReader& reader) {
        size_t read;

        setupForReader(reader);

        while ((read = reader.read(&buffer[0], buffer.size()))) {
            try {
                size_t written = pcm.write(&buffer[0], read);

                if (written != buffer.size()) {
                    log << "Short write" << std::endl;
                }
            } catch (AlsaException & e) {
                if (e.isFatal()) {
                    throw;
                } else {
                    log << e.what() << std::endl;
                }
            }
            #ifdef USE_WDT
                wdt.kick();
            #endif
            if(stop) {
                break;
            }
        }

        pcm.drain();
        reader.rewind();
    }

    void noFile() {
        for(int i = 0; i < 10; i++) {
            play(errorNoFile);
        }
    }

    void badFile() {
        for(int i = 0; i < 10; i++) {
            play(errorBadFile);
        }
    }

};

void sigterm(int sig) {
    stop = true;
}

int main(int argc, char** argv) {
    std::string prefix(INSTALL_PREFIX);

    if (argc != 3) {
        std::cout << "Usage: player FILENAME LOG_FILENAME" << std::endl;
        return 1;
    }

    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = sigterm;
    sigaction(SIGTERM, &action, NULL);

    std::ofstream log(argv[2], std::ios_base::out | std::ios_base::app);

    try {
        Player p(argv[1], prefix, log);
        p.run();
    } catch(WavReaderException & e) {
        log << e.what() << strerror(e.getErrno()) << std::endl;
        return 1;
    } catch(AlsaException& e) {
        log << e.what() << strerror(e.getErrno()) << std::endl;
        return 1;
    } catch(WatchdogException& e) {
        log << e.what() << strerror(e.getErrno()) << std::endl;
        return 1;
    }
    return 0;
}
