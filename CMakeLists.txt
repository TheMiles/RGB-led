#=============================================================================#
# Author: QueezyTheGreat                                                      #
# Date:   26.04.2011                                                          #
#                                                                             #
# Description: Arduino CMake example                                          #
#                                                                             #
#=============================================================================#
set(CMAKE_TOOLCHAIN_FILE cmake/ArduinoToolchain.cmake) # Arduino Toolchain


cmake_minimum_required(VERSION 2.8)
#====================================================================#
#  Setup Project                                                     #
#====================================================================#
project(ArduinoExample C CXX)

print_board_list()
print_programmer_list()

#set(ARDUINO_DEFAULT_BOARD atmega328) # Default Board ID, when not specified
#set(ARDUINO_DEFAULT_PORT /dev/ttyUSB0) # Default Port, when not specified

generate_arduino_firmware(RGB_serial
                          SKETCH .
                          PORT /dev/ttyUSB0
                          BOARD atmega328
                         )
