This project is a paired [Arduboy](https://arduboy.com) firmware and Python script to use the
Arduboy as a remote to control playback on [Music Player Daemon (MPD)](https://www.musicpd.org/).

To install on the Arduboy, simply load ``MPDControl.ino`` in the 
[Arduino IDE](https://www.arduino.cc/en/software) and load to the Arduboy per normal.
The firmware should not
need any tailoring, but is rather simple if you want to tweak it.

Note: in any steps listed in this readme, # is for a step performed by root or sudo,
while $ is for a step run by a normal user.

# Dependencies

The Python script requires the [python-mpd2](https://github.com/Mic92/python-mpd2) and
[pyserial](https://github.com/pyserial/pyserial) libraries.

On Debian/Ubuntu or derivatives, these packages can be installed as:

    # apt install python3-mpd python3-serial

Or you can use pip:

    $ pip install python-mpd2 pyserial

# Direct use

Basic use consists of running the Python script while the Arduboy is
connected to the same computer and running the MPDControl firmware.

    $ ./mpdproxy.py

Running the script with no arguments assumes that MPD is running on the
same PC as the proxy, is using the default port of **6600**, and
that the Arduboy serial port is **/dev/ttyACM0**. Any of these
can be changed using command line arguments. E.g.:

    $ ./mpdproxy.py --server delta.local --port 1234 --dev /dev/ttyUSB0
    
There is also a parameter to change the baud rate, if you decide to update
that in the firmware.

As per most serial things, your user will need to be a member of the **dialout** group
to use the port, which can be set as follows:

    # usermod -a -G dialout username

# Use as a service

I have also included a systemd service file, mpdproxy.service to keep
the proxy running. This will allow you to simply connect and turn on
the Arduboy to use as a remote at any time.

Basic install method is as follows, using systemctl link, install
of proxy to /usr/local/libexec, mpd instance on the same machine as the
proxy, and using the mpd user. If any of these assumptions don't match
your setup or preferred install, you may need to edit the service file
and adapt some lines below according.

    # install mpdproxy.py /usr/local/libexec/mpdproxy.py
    # usermod -a -G dialout mpd
    # systemctl link /path/to/MPDControl/mpdproxy.service
    # systemctl start mpdproxy


