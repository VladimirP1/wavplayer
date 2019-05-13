Player 1.0
---------------------------

This program was created for a project which reqired uninterrupted cyclical playback of 
simple WAV files form a USB drive. There was a requerement for maximum fault-tolarancy, hence 
the standard linux watchdog is used to cope with any hangs or lockups, which may occur in an 
embedded linux system.

To use this program you should build it and install (`ninja install`), add an entry to `/etc/fstab`
for the `/mnt/USB` mountpoint and enable the `player.service` systemd service, installed with this player.

Then make a reboot, and you will see further documentation to appear on the drive. 

Please note, that only integer PCM formats are supported.
Also, you should have a proper alsa sound card and a watchdog device at `/dev/watchdog` for everything 
to work correctly
