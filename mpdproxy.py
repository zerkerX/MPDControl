#!/usr/bin/python3
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#  
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
import struct
import pdb
import time
import os
from argparse import ArgumentParser
from mpd import MPDClient
from serial import Serial
from serial.serialutil import SerialException

class MPDProxy:
    # Commands from the Arduboy (as single-lenghth byte strings):
    CMDIDLE = b'\x00'
    CMDVOLUP = b'\x01'
    CMDVOLDOWN = b'\x02'
    CMDNEXT = b'\x03'
    CMDPREV = b'\x04'
    CMDPLAYPAUSE = b'\x05'
    CMDSOMETHING = b'\x06'
    
    STATES = {
        "stop" : 0,
        "play" : 1,
        "pause" : 2,
    }

    def __init__(self, args):
        self.server = args.server
        self.port = args.port
        self.dev = args.dev
        self.baud = args.baud
        self.conn = None
        self.lastid = -1
    
    def __do_cmd(self, cmd, status):
        if cmd == self.CMDIDLE:
            return
        elif cmd == self.CMDVOLUP:
            self.conn.volume(5)
        elif cmd == self.CMDVOLDOWN:
            self.conn.volume(-5)
        elif cmd == self.CMDPLAYPAUSE:
            if status['state'] == 'play':
                self.conn.pause()
            else:
                self.conn.play()
        elif cmd == self.CMDNEXT:
            self.conn.next()
        elif cmd == self.CMDPREV:
            self.conn.previous()
        elif cmd == self.CMDSOMETHING:
            # No idea what to have the other button do.
            pass
    
    def __send_status(self, ser, status):
        song = self.conn.currentsong()
        sendinfo = self.lastid != status['songid']
        
        # Some fields are optional when stopped, so handle those cleanly
        elapsed = int(float(status['elapsed'])) if 'elapsed' in status else -1
        vol = int(status['volume']) if 'volume' in status else -1
        duration = int(float(status['duration'])) if 'duration' in status else 0
        
        ser.write(struct.pack('<hhBB',
            vol, elapsed,
            self.STATES[status['state']],
            int(sendinfo)))

        if sendinfo:
            ser.write(struct.pack('<H21s21s21s',
                duration,
                song['title'].encode('cp437', errors='replace'),
                song['artist'].encode('cp437', errors='replace'),
                song['album'].encode('cp437', errors='replace')))

        self.lastid = status['songid']
    
    def __process_loop(self):
        self.conn = MPDClient()
        self.conn.timeout = 10
        self.conn.connect(self.server, self.port)
        self.lastid = -1
        
        print("Connected")

        # Mix of try/finally and with, since MPDClient doesn't support the latter
        try:
            with Serial(self.dev, self.baud, timeout=1) as ser:
                while True:
                    key = ser.read()
                    if key == b'\xa5':
                        cmd = ser.read()
                        status = self.conn.status()
                        self.__do_cmd(cmd, status)
                        self.__send_status(ser, status)
        finally:
            self.conn.close()
            self.conn.disconnect()  
    
    def run(self):
        print ("Started")
        while True:
            if os.path.exists(self.dev):
                try:
                    self.__process_loop()
                except SerialException:
                    # Probably got disconnected
                    print("Disconnected")
                    pass
            else:
                time.sleep(5)

if __name__ == "__main__":
    parser = ArgumentParser(description='Proxy to control MPD from '
        'device connected on serial port (e.g. Arduboy or similar).\n'
        'NOTE: This program is designed to run forever; use ctrl + C '
        'to quit.')
    parser.add_argument('--server', '-s', default='localhost',
                       help='MPD server to connect to')
    parser.add_argument('--port', '-p', default=6600,
                       help='MPD port to use')
    parser.add_argument('--dev', '-d', default='/dev/ttyACM0',
                       help='Serial port device to use for communication')
    parser.add_argument('--baud', '-b', default=19200,
                       help='Baud rate to use')
    proxy = MPDProxy(parser.parse_args())
    proxy.run()
