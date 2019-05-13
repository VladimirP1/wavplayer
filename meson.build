project('player', ['cpp', 'c'],        version: '0.1.0',
  meson_version: '>= 0.36.0',
)

prefix = get_option('prefix')
add_project_arguments('-DINSTALL_PREFIX="' + prefix + '"', language : 'cpp')

incdir = include_directories('inc')

wavreader = static_library('wavreader', ['src/wavreader.c', 'src/wavreaderpp.cpp', 'src/alsa.cpp', 'src/wdt.cpp'], include_directories : incdir)

install_data('res/start.sh', install_mode: 'rwxr-xr-x', install_dir: 'share/player')
install_data('res/player.service', install_dir: '/etc/systemd/system', install_mode: 'rw-r--r--')
install_data(['res/music.wav', 'res/sine.wav', 'res/error_no_file.wav', 'res/error_bad_file.wav', 'res/README.txt', 'res/screenshot.png'], install_dir: 'share/player')


alsa = dependency('alsa')

executable('player', ['src/main.cpp'], link_with : [wavreader], dependencies: [alsa], include_directories: incdir, install:true)
