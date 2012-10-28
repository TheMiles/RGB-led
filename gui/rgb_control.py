#!/usr/bin/python -tt
#
# a simple gui to control the color values of a RGBB Led
#
# Mostly reworking the example form Fabian Affolter
# http://www.gitorious.org/arduino-led
#
#
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
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import os
import sys
import serial
import time
import signal
from gi.repository import Gtk
from gi.repository import GObject

SPEED = 9600


class Channel:
    Red, Green, Blue1, Blue2 = range(4)

# class LedChannel( Gtk.Adjustment ):
#     __gproperties__ = {
#         "channel": (int, # type
#                      "led channel", # nick
#                      "The channel number", # blurb
#                      0, # min
#                      4, # max
#                      0, # default
#                      GObject.PARAM_READABLE # flags
#                     ),
#     }

    
#     def __init__(self, channel, adjustment ):
#         Gtk.Adjustment.__init__(adjustment)
#         self.channel = channel

#     def get_channel(self):
#         return self.channel
        
#     def do_get_property(self, prop):
#         if prop.name == 'channel':
#             return self.channel
#         else:
#             raise AttributeError, 'unknown property %s' % prop.name

#     def do_set_property(self, prop, value):
#         if prop.name == 'channel':
#             self.channel = value
#         else:
#             raise AttributeError, 'unknown property %s' % prop.name

# GObject.type_register(LedChannel)

class MainWindow:
    
    def __init__(self):
        self.builder = Gtk.Builder()
        self.builder.add_from_file(os.path.join(os.getcwd(), 'data/rgb_control.ui'))
        self.window = self.builder.get_object('window1')
        self.channelRed = self.builder.get_object('adjustmentRed')
        self.channelGreen = self.builder.get_object('adjustmentGreen')
        self.channelBlue1 = self.builder.get_object('adjustmentBlue1')
        self.channelBlue2 = self.builder.get_object('adjustmentBlue2')
        self.menu_quit     = self.builder.get_object('imagemenuitem15')

        self.window.connect('delete-event', self.quit)
        self.channelRed.connect('value_changed', self.channelChanged, Channel.Red )
        self.channelGreen.connect('value_changed', self.channelChanged, Channel.Green )
        self.channelBlue1.connect('value_changed', self.channelChanged, Channel.Blue1 )
        self.channelBlue2.connect('value_changed', self.channelChanged, Channel.Blue2 )
        self.menu_quit.connect('activate', self.quit)

        port = self.builder.get_object('serialPort').get_text()
        self.connection = serial.Serial( port, 
                                    SPEED,
                                    timeout=0,
                                    stopbits=serial.STOPBITS_TWO
                                    )

        self.window.show()
        

    def channelChanged(self, adjustment, channel):
        self.send_command( "c " + str( channel ) )
        self.send_command( "v " + str(adjustment.get_value()) )

    def quit(self, *args):
        Gtk.main_quit()

    def send_command(self, val):
        self.connection.write(val)
        #connection.close()

if __name__ == '__main__':
    signal.signal(signal.SIGINT, signal.SIG_DFL)
    signal.signal(signal.SIGTERM, signal.SIG_DFL)
    MainWindow()
    Gtk.main()
