#!/usr/bin/python

import sys
import serial
import time
import atexit

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

fd = open('data','w') # plot "data" using 1:2 with linespoints, "data" using 1:4 with lines



count = 0
fx, fy, fz = 0, 0, 0
filter_level = 5
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

        process(fx, fy, fz, qd)
        
        fx = fy = fz = 0
        count = 0


start_time = time.time()
started = False
last_qd = 0
def process(fx, fy, fz, qd):
    
    global start_time, started, last_qd
    print round(time.time(), 4), '\t\t',
    print 'filted:', fx,'\t\t', fy,'\t\t', fz,'\t\t', qd
    
    if qd != 0:
        if started is False:
            #fd.write('elapse\taccl x\tqd\t,qd delta\n')
            started = True
            start_time = time.time()

        fd.write('%f\t%d\t%d\t%d\n'%(time.time()-start_time, fx, qd, abs(qd-last_qd)))
        last_qd = qd

    if abs(qd) > (4096 * 3.5):
        print 'reached'
        sys.exit(0)

    
    
    elapse = time.time()
    

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
