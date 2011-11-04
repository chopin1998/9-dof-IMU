#!/usr/bin/python

import sys
import serial


def main():

    dev = serial.Serial('/dev/rfcomm0', 115200)

    IN_MESURE = False
    while True:

        key = raw_input('---> ')
        if key in ('q', 'Q'):
            break



################################:
if __name__ == '__main__':

    main()
