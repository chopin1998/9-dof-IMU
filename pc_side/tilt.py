#!/usr/bin/python

import sys
import serial
import time
import atexit
import math

def close_dump():
    print 'close dump'
    dev.write('[accl,dump,0]')

atexit.register(close_dump)

dev = serial.Serial('/dev/ttyUSB0', 576000, timeout=1)
time.sleep(0.5)
dev.flush()
dev.write('[qd,clear]')
dev.write('[accl,dump]')
dev.flush()


count = 0
fx, fy, fz = 0, 0, 0
filter_level = 64
def data_filter(data):

    #process(*data)
    #return
    
    global count, fx, fy, fz, filter_level

    x,y,z,qd = data
    if (count < filter_level):
        fx += x
        fy += y
        fz += z

        count += 1
        #print x,'\t',y,'\t',z,'\t', qd
    else:
        fx /= filter_level
        fy /= filter_level
        fz /= filter_level

        process(fx, fy, fz)
        
        fx = fy = fz = 0
        count = 0


def process(fx, fy, fz):
    
    print 'filted:', fx,'\t', fy,'\t', fz, '\t--->\t',

    r = (fx ** 2 + fy ** 2 + fz ** 2) ** 0.5
    axr = math.asin(fx/r) * 180 / math.pi
    ayr = math.asin(fy/r) * 180 / math.pi
    azr = math.asin(fz/r) * 180 / math.pi

    print 'axr: %f\t\tayr: %f\t\t, azr: %f' %(axr, ayr, azr)

while True:
    
    raw = dev.readline().strip()
    if not raw:
        print 'no data'
        continue

    try:
        data = raw.split(':')[1]
        data = map(int, data.split('|'))
    except Exception ,ex:
        print '!!!!!!!!!!!!!!!! error data', raw, ex
        continue

    data_filter(data)

raw_input()
