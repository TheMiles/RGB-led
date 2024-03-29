#!/usr/bin/python -tt
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

import os, sys

import serial

from gi.repository import Gtk
from gi.repository import Gdk
from gi.repository import GObject

from threading import Thread

Gdk.threads_init()

SPEED = 9600

class MainWindow:
    
    def __init__(self):

        self.builder = Gtk.Builder()
        self.builder.add_from_file(os.path.join(os.getcwd(), 'data/rgb_control.ui'))
        self.builder.connect_signals(self)

        self.quitting = False

        channel = 0
        while True:
            try:
                adj = self.builder.get_object('channel_%i' % channel)
                adj.channel = channel
                channel += 1
            except:
                break

        self.connect_serial(self.builder.get_object('button1'))

        self.read_thread = Thread(target=self.receive_data)
        self.read_thread.start()

        self.window = self.builder.get_object('window1')
        self.window.show()
        

    def channel_changed(self, adjustment):

        self.send_command("c%i" % adjustment.channel)
        self.send_command("v%i" % adjustment.get_value())


    def connect_serial(self, button):

        controls = self.builder.get_object('grid1').get_children()
        def set_sensitive(sensitive):
            for c in controls:
                c.set_sensitive(sensitive)

        def disconnect():
            button.set_label("Connect")
            set_sensitive(False)
            self.connection = None

        if button.get_label() == "Disconnect":
            disconnect()
            return

        try:
            port = self.builder.get_object('serialPort').get_text()
            self.connection = serial.Serial(port, SPEED, timeout=0, stopbits=serial.STOPBITS_ONE)
            button.set_label("Disconnect")
            set_sensitive(True)
        except serial.serialutil.SerialException, e:
            disconnect()
            print e
        

    def quit(self, *args):
        self.connection = None
        self.quitting = True
        Gtk.main_quit()

    def send_command(self, val):
        self.connection.write(val)

    def receive_data(self):

        buffer = ""
        while True:
            if self.connection:
                buffer += self.connection.read(self.connection.inWaiting())
                buffer_list = buffer.split('\n')
                for line in map(lambda x: x.replace("\r", ""), buffer_list[:-1]):
                    GObject.idle_add(self.update_data_buffer, line)
                    print line
                buffer = buffer_list[-1]
            if self.quitting:
                break


    def update_data_buffer(self, data):

        textview = self.builder.get_object('textview1')
        data_buffer = Gtk.TextBuffer()
        textview.set_buffer(data_buffer)
        scrollbar = self.builder.get_object('scrolledwindow1').get_vadjustment()

        data_buffer.insert(data_buffer.get_end_iter(), data)
        #scrollbar.set_value(scrollbar.upper - scrollbar.page_size)


if __name__ == '__main__':

    import signal
    signal.signal(signal.SIGINT, signal.SIG_DFL)
    signal.signal(signal.SIGTERM, signal.SIG_DFL)

    MainWindow()
    Gtk.main()
    
