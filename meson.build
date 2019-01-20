project('player', ['cpp', 'c'],        version: '0.1.0',
  meson_version: '>= 0.40.0',
)

wavreader = static_library('wavreader', ['wavreader.c', 'wavreaderpp.cpp', 'alsa.cpp', 'wdt.cpp'])

install_data('start.sh', install_mode: 'rwxr-xr-x')
install_data('player.service', install_dir: '/etc/systemd/system', install_mode: 'rw-r--r--')
install_data(['music.wav', 'sine.wav', 'error_no_file.wav', 'error_bad_file.wav'])


alsa = dependency('alsa')

executable('player', ['main.cpp'], link_with : [wavreader], dependencies: [alsa], install : true)
