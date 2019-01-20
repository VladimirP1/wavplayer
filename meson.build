project('player', ['cpp', 'c'],        version: '0.1.0',
  meson_version: '>= 0.36.0',
)

prefix = get_option('prefix')
add_project_arguments('-DINSTALL_PREFIX="' + prefix + '"', language : 'cpp')

wavreader = static_library('wavreader', ['wavreader.c', 'wavreaderpp.cpp', 'alsa.cpp', 'wdt.cpp'])

install_data('start.sh', install_mode: 'rwxr-xr-x', install_dir: 'share/player')
install_data('player.service', install_dir: '/etc/systemd/system', install_mode: 'rw-r--r--')
install_data(['music.wav', 'sine.wav', 'error_no_file.wav', 'error_bad_file.wav', 'README.txt', 'screenshot.png'], install_dir: 'share/player')


alsa = dependency('alsa')

executable('player', ['main.cpp'], link_with : [wavreader], dependencies: [alsa], install:true)
